//
// Created by 张志龙 on 2020/6/6.
//

#include "EventLoopThread.h"

#include <utility>
#include "net/EventLoop.h"

using namespace server::net;
using namespace server;

EventLoopThread::EventLoopThread(server::net::EventLoopThread::ThreadInitCallback cb,
                                 const std::string &name)
        : loop_(nullptr),
          exiting_(false),
          thread_([this] { threadFunc(); }, name),
          mutex_(),
          cond_(mutex_),
          callback_(std::move(cb)) {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {  //此时threadFunc可能在执行回调
        // 有小概率，threadFunc正在执行，而此时调用了该类析构函数，致使程序异常退出
        loop_->quit();
        thread_.join();
    }

}

EventLoop *EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();   //这个是主线程，返回子线程EventLoop对象的地址loop_*
    EventLoop *loop = nullptr;
    {
        MutexLockGuard lock(mutex_);
        while (loop_ == nullptr) {
            cond_.wait();
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;         //这里是子线程,子线程运行loop
    if (callback_) {
        callback_(&loop);
    }
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    MutexLockGuard lock(mutex_);
    loop_ = nullptr;
}
