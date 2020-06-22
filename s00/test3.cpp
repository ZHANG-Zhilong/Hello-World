//
// Created by 张志龙 on 2020/6/3.
//

#include "net/EventLoop.h"
#include "net/Channel.h"
#include "base/Timestamp.h"
#include <sys/timerfd.h>
#include <cstdio>

using namespace server;
using namespace server::net;

EventLoop *g_loop;

void timeout(Timestamp receivetime) {
    printf("%s Timeout!\n", receivetime.toFormattedString().c_str());
    g_loop->quit();
}

int main() {
    printf("%s started\n", server::Timestamp::now().toFormattedString().c_str());
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();
    struct itimerspec howlong{};
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 2;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    loop.loop();
    close(timerfd);
    return 0;
}

