//
// Created by 张志龙 on 2020/6/4.
//

#include "base/Logging.h"
#include "net/TimerQueue.h"
#include "net/EventLoop.h"
#include "net/Timer.h"
#include "net/TimerId.h"
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

//EventLoop 调用runAt系列函数，以不同定时方式注册回调，将回调以Timer形式注册到TimerQueue中
//到期时Expiration触发可读事件，逐一调用到期Timer注册的回调

namespace server {

    namespace net {
        namespace detail {
            int createTimerfd() {
                int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                               TFD_NONBLOCK | TFD_CLOEXEC);
                if (timerfd < 0) {
                    LOG_SYSFATAL << "Failed in timerfd_create";
                }
                return timerfd;
            }

            struct timespec howMuchTimeFromNow(Timestamp when) {
                int64_t microseconds = when.microSecondsSinceEpoch() -
                                       Timestamp::now().microSecondsSinceEpoch();
                if (microseconds < 100) {
                    microseconds = 100;
                }
                struct timespec ts{};
                ts.tv_sec = static_cast<time_t>(
                        microseconds / Timestamp::kMicroSecondsPerSecond);
                ts.tv_nsec = static_cast<long>(
                        (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000 );
                return ts;
            }

            void readTimerfd(int timerfd, Timestamp now) {
                uint64_t howmany;
                ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
                LOG_TRACE << "TimerQueue::handleRead() called" << howmany << " at " << now.toString();
                if (n != sizeof howmany) {
                    LOG_ERROR << "TimerQueue::handleRadk() reads " << n << "bytes instead of 8";
                }
            }

            void resetTimerfd(int timerfd, Timestamp expiration) {
                //wakeup loop by timerfd_settime()
                struct itimerspec newValue{};
                struct itimerspec oldValue{};
                memZero(&newValue, sizeof newValue);
                memZero(&oldValue, sizeof oldValue);
                newValue.it_value = howMuchTimeFromNow(expiration);
                int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
                if (ret) {
                    LOG_SYSERR << "timerfd_settime()";
                }
            }
        }
    }
}

using namespace server;
using namespace server::net;
using namespace server::net::detail;

//初始化时设置可读回调 TimerQueue::handleRead()
TimerQueue::TimerQueue(EventLoop *loop)
        : loop_(loop),
          timerfd_(createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          timers_(),
          callingExpiredTimers_(false) {
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    auto *pTimer = reinterpret_cast<Timer *>(UINTPTR_MAX);
    Entry sentry = std::make_pair(now, pTimer);
    auto it = timers_.lower_bound(sentry);  //返回第一个未到期的timer
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, back_inserter(expired));
    return expired;
}


TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    //不必移除channel，我们正在channel析构函数中
    //timers_里面的timer不必手动回收，已经托管给unique_ptr了
}

TimerId TimerQueue::addTimer(const TimerQueue::TimeCallback &cb,
                             Timestamp when, double interval) {
    //转发回调
    std::unique_ptr<Timer> timer(new Timer(cb, when, interval));
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer.get()));
    return {timer.get(), timer->sequence()};
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    //修改定时器列表
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::handleRead() {
    //触发可读，获取到期TimerList，逐一调用构造Timer时注册的回调
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (const Entry &it: expired) {
        it.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {

    Timestamp nextExpire;

    for (const Entry &it: expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat()
            && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            it.second->restart(now);
            insert(it.second);
        } else {
            delete it.second;    /// FIXME  move to a free list.
        }
    }

    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        resetTimerfd(timerfd_, nextExpire);
    }

}


bool TimerQueue::insert(Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    auto it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result =
                timers_.insert(Entry(when, timer));
        assert(result.second);
        (void) result;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result =
                activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
        (void) result;
    }
    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}

void TimerQueue::cancel(TimerId timerId) {
    loop_->runInLoop(
            std::bind(&TimerQueue::cancelInLoop, this, timerId)
    );
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1);
        (void) n;
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        /* 自注销  在定时器回调中注销当前定时器
         * 为了应对这这情况，会记住在本次调用到期Timer期间，有哪些cancel请求，
         * 不再把已经cancel()的Timer添加回timers_和activeTimers_中
         * 涉及api: handleRead(),reset()*/
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
}













