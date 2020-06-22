//
// Created by 张志龙 on 2020/6/6.
//

#ifndef SERVER_EVENTLOOPTHREAD_H
#define SERVER_EVENTLOOPTHREAD_H

#include <functional>
#include "base/noncopyable.h"
#include "base/Mutex.h"
#include "base/Condition.h"
#include "base/Thread.h"

namespace server {
    namespace net {
        //IO线程不一定是主线程，我们可以在任何一个线程创建并运行EventLoop，
        // 一个程序也可以有不止一个IO线程
        //one loop per thread
        class EventLoop;

        class EventLoopThread {
        public:
            using ThreadInitCallback = std::function<void(EventLoop *)>;

            explicit EventLoopThread(ThreadInitCallback cb = ThreadInitCallback(),
                                     const std::string &name = std::string());

            ~EventLoopThread();

            EventLoop *startLoop(); //返回新线程中EventLoop对象地址

        private:
            //线程主函数在stack上定义EventLoop对象，然后将其地址给loop_成员变量
            //最后notify()条件变量唤醒startLoop()，ThreadFunc()退出后EventLoop对象指针失效
            void threadFunc();

            Thread thread_;

            MutexLock mutex_;
            EventLoop *loop_ GUARDED_BY(mutex_);
            Condition cond_ GUARDED_BY(mutex_);
            bool exiting_;

            ThreadInitCallback callback_;  //构造函数初始化回调函数
        };
    }
}


#endif //SERVER_EVENTLOOPTHREAD_H
