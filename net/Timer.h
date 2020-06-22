//
// Created by 张志龙 on 2020/6/4.
//

#ifndef SERVER_TIMER_H
#define SERVER_TIMER_H

#include "base/Timestamp.h"
#include "base/Atomic.h"
#include "net/Callbacks.h"
#include "base/Timestamp.h"

namespace server {
    namespace net {

        /*****************
         * 管理时间戳Timestamp、TimerCallback、标记时间戳序号sequence、控制重复计时⌛️、计时有效性
         * run():void                🦅调用回调函数
         * restart(Timestamp):void   重新⌛️计时（若重复计时，有效，否则返回无效Timestamp
         * repeat():bool             间隔⌛️计时
         * sequence():int64_t        时间戳序号
         * expiration():Timestamp    🔙返回时间戳
         * numCreate():int64_t       标记序号
         */

        class Timer : public noncopyable {
            //无法区分地址相同的先后两个Timer对象，因此生成全局递增序号
            //有回调，有计时，有序号
        public:
            Timer(TimerCallback cb, Timestamp when, double interval)
                    : callback_(std::move(cb)),
                      expiration_(when),
                      interval_(interval),
                      repeat_(interval > 0.0),
                      sequence_(s_numCreated_.incrementAndGet()) {
            }

            void run() const { callback_(); }

            void restart(Timestamp now);

            bool repeat() const { return repeat_; }

            int64_t sequence() const { return sequence_; }

            Timestamp expiration() const { return expiration_; }

            static int64_t numCreate() { return s_numCreated_.get(); }

        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            const double interval_;
            const bool repeat_;
            const int64_t sequence_;
            static AtomicInt64 s_numCreated_;
        };
    }
}


#endif //SERVER_TIMER_H
