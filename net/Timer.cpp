//
// Created by 张志龙 on 2020/6/4.
//

#include "Timer.h"

using namespace server;
using namespace server::net;

AtomicInt64  Timer::s_numCreated_;

void Timer::restart(Timestamp now) {
    if (repeat()) {
        this->expiration_ = addTime(now, interval_);
    } else {
        expiration_ = Timestamp::invalid();
    }

}

