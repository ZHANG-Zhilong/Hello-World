#include <iostream>
#include <cstdio>
#include "net/EventLoop.h"
#include <thread>
#include <pthread.h>

void threadFunc() {
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), (int) server::CurrentThread::tid());
    server::net::EventLoop loop;
    loop.loop();
}

int main() {
    return 0;
}
