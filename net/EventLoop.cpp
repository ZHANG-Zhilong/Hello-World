//
// Created by 张志龙 on 2020/5/31.
//

#include "net/Poller.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/TimerId.h"
#include "net/TimerQueue.h"
#include "net/SocketsOps.h"
#include <algorithm>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>


using namespace server;
using namespace server::net;

namespace {
    __thread EventLoop *t_loopInThisThread = nullptr;
    const int kPollTimeMs = 10000;

    int createEventfd() {
        int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            LOG_TRACE << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }

#pragma GCC diagnostic ignored "-Wold-style-cast"

    class IgnoreSigPipe {
    public:
        IgnoreSigPipe() {
            ::signal(SIGPIPE, SIG_IGN);
            LOG_TRACE << "Ignore SIGPIPE";
        }
    };

#pragma GCC diagnostic error "-Wold-style-cast"
    IgnoreSigPipe initObj;
}


EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    //调用Poll::poll()获取当前活动事件的channelLIst列表
    //再一次调用每个Channel的handleEvent函数
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (auto activeChannel : activeChannels_) {
            activeChannel->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping.";
    looping_ = false;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

EventLoop::EventLoop() :
        looping_(false),
        poller_(Poller::newDefaultPoller(this)),
        wakeupFd_(createEventfd()),
        quit_(false),
        eventHandling_(false),
        callingPendingFunctors_(false),
        threadId_(CurrentThread::tid()),
        currentActiveChannel_(nullptr),
        wakeupChannel_(new Channel(this, wakeupFd_)) {
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread" << threadId_;
    } else {
        t_loopInThisThread = this;
    }
}

void EventLoop::assertInLoopThread() {
    if (!isInLoopThread()) {
        LOG_FATAL << "called outside its owner thread";
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    this->assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(),
                         channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

void EventLoop::quit() {
    this->quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(const EventLoop::Functor &cb) {
    //在它的IO线程内执行用户回调，分两种情况：
    //1）用户在当前线程调用，立即执行
    //2）用户在其他线程调用，则加入队列，IO线程被唤醒执行该回调
    if (isInLoopThread()) {
        cb();
    } else {
        this->queueInLoop(cb);
    }


}

TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback &cb) {
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback &cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback &cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
    MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(cb);
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8.";
    }
}

void EventLoop::handleRead() const {
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8.";
    }
}

void EventLoop::doPendingFunctors() {  /* 把回调列表交换到局部变量functors中 */
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (const auto &func: functors) {
        func();
    }
    callingPendingFunctors_ = false;
}



