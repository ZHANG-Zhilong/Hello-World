//
// Created by 张志龙 on 2020/5/31.
//

#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include "net/Callbacks.h"
#include "base/noncopyable.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Timestamp.h"
#include "base/Mutex.h"
#include <cassert>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <memory>
#include <atomic>
#include <functional>

//每一个线程拥有自己的eventloop，能够检查调用eventloop的是不是其拥有的线程
namespace server {
    namespace net {
        class TimerId;

        class Poller;

        class TimerQueue;

        class Channel;

        /***********
        * EventLoop 通过 unique_ptr 间接持有Poller，持有Channel* 数组，
        * EventLoop 通过 unique_ptr 间接持有Poller，持有Channel* 数组，
        * Poller    负责ChannelList事件分发，EventLoop维持activeChannels_（活跃的Channels）
        * EventLoop 通过 unique_ptr 间接持有Poller，持有Channel* 数组，
        * EventLoop 通过 unique_ptr 间接持有Poller，持有Channel* 数组，
        * EventLoop::Loop   调用Poller::poll获取活跃activeChannels_，
        *                      调用Channel::handleEvents()处理各自事件
        */

        class EventLoop : server::noncopyable {

            using Functor = std::function<void()>;

        public:
            EventLoop();

            ~EventLoop();

            void loop();

            void quit();

            void runInLoop(const Functor &cb);  //有其他任务让loop执行

            void queueInLoop(const Functor &cb);

            TimerId runAt(const Timestamp &time, const TimerCallback &cb);

            TimerId runAfter(double delay, const TimerCallback &cb);

            TimerId runEvery(double interval, const TimerCallback &cb);

            //internal usage
            void wakeup() const;

            void updateChannel(Channel *channel);

            void removeChannel(Channel *channel);

            //assertion
            void assertInLoopThread();

            bool isInLoopThread() const;

        private:
            void abortNotInLoopThread();

            void handleRead() const;  //wakeup
            void doPendingFunctors();

            typedef std::vector<Channel *> ChannelList;

            bool looping_{false};                      /* atomic */
            std::atomic<bool> quit_;
            bool eventHandling_;                /* atomic */
            bool callingPendingFunctors_{false};       /* atomic */
            const pid_t threadId_;
            Timestamp pollReturnTime_;
            std::unique_ptr<Poller> poller_;    /* 间接持有Poller */
            std::unique_ptr<TimerQueue> timerQueue_;
            int wakeupFd_{-1};

            /* 处理wakeupFd_的可读事件，并分发到handleRead函数 */
            std::shared_ptr<Channel> wakeupChannel_;

            ChannelList activeChannels_;
            Channel *currentActiveChannel_;

            mutable MutexLock mutex_;
            /* pendingFunctors暴露给其他线程 */

            //guarded by mutex_
            std::vector<Functor> pendingFunctors_ GUARDED_BY(mutex_);
        };
    }
}
#endif //SERVER_EVENTLOOP_H
