#include <cstdio>
#include <unistd.h>
#include "base/CurrentThread.h"
#include "net/EventLoop.h"
#include <thread>

using namespace server;
using namespace server::net;
EventLoop * g_loop;
void threadFunc(){
    printf("threadFunc(): pid:%d, tid:%d\n", getpid(), server::CurrentThread::tid());
    g_loop->loop();

}

int main(){
    EventLoop loop;
    g_loop = &loop;
    std::thread t (threadFunc);
    t.join();
    return 0;
}