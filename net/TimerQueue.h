//
// Created by 张志龙 on 2020/6/4.
//

#ifndef SERVER_TIMERQUEUE_H
#define SERVER_TIMERQUEUE_H

#include "base/noncopyable.h"
#include "net/Channel.h"
#include "base/Timestamp.h"
#include <set>
#include <memory>
#include <vector>


namespace server {

    namespace net {

        class EventLoop;

        class TimerId;

        class Timer;

        class TimerQueue : public noncopyable {
        public:
            typedef std::function<void()> TimeCallback;

            explicit TimerQueue(EventLoop *eventloop);

            ~TimerQueue();

            /*********
             * Schedules the callback to be run at given time.
             * repeat if @interval >0.0
             * Must be thread safe. Usually called from other threads.
             */
            //转发事件
            TimerId addTimer(const TimeCallback &cb, Timestamp when, double interval);


            //thread safe.
            void cancel(TimerId timerId);

        private:
            //here can use unique_ptr instead of naked/raw pointer.
            using Entry = std::pair<Timestamp, Timer *>;
            using TimerList = std::set<Entry>;
            using ActiveTimer = std::pair<Timer *, int64_t>;
            using ActiveTimerSet = std::set<ActiveTimer>;

            //修改定时器列表
            void addTimerInLoop(Timer *timer);

            void cancelInLoop(TimerId timerId);

            //called when timerfd alarms.
            void handleRead();

            //move out all expired timers
            std::vector<Entry> getExpired(Timestamp now);

            void reset(const std::vector<Entry>& expired, Timestamp now);

            bool insert(Timer *timer);

            EventLoop *loop_;
            const int timerfd_;
            Channel timerfdChannel_;   //handleRead()
            TimerList timers_;  //sorted by expiration.

            /* for cancel */
            /* TimerId 不负责Timer的生命期， 其中保存的Timer*可能失效，因此不能直接解引用
             * 只有在activeTimers_中找到了Timer时才可以提领 */
            bool callingExpiredTimers_;
            ActiveTimerSet activeTimers_;
            ActiveTimerSet cancelingTimers_;
        };
    }
}


#endif //SERVER_TIMERQUEUE_H
