//
// Created by 张志龙 on 2020/6/4.
//

#ifndef SERVER_TIMERID_H
#define SERVER_TIMERID_H

#include <cstdint>
#include "base/copyable.h"

namespace server {
    namespace net {

        class Timer;

        class TimerId : public copyable {
        public:
            TimerId() : timer_(nullptr), sequence_(0) {}

            TimerId(Timer *pTimer, int64_t seq)
                    : timer_(pTimer), sequence_(seq) {}

            friend class TimerQueue;

        private:
            Timer *timer_;
            int64_t sequence_;
        };

    }
}


#endif //SERVER_TIMERID_H
