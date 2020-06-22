//
// Created by 张志龙 on 2020/6/8.
//

#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include "base/noncopyable.h"
#include "net/Callbacks.h"
#include "base/Atomic.h"
#include <functional>
#include <map>

namespace server {
    namespace net {
        class InetAddress;

        class Acceptor;

        class EventLoop;

        class EventLoopThreadPool;

        class TcpServer {
            //TcpServer向TcpConnection注册CloseCallback， 用于接受链接断开的信息
        public:
            using ThreadInitCallback = std::function<void(EventLoop *)>;
            enum Option {
                kNoReusePort, kReusePort
            };

            TcpServer(EventLoop *loop, const InetAddress &listenAddr, string nameArg);

            ~TcpServer();

            void setThreadNum(int numThreads);

            void setThreadInitCallback(const ThreadInitCallback &cb) {
                threadInitCallback_ = cb;
            }

            std::shared_ptr<EventLoopThreadPool> threadPool() {
                return threadPool_;
            }

            void start();

            void setConnectionCallback(const ConnectionCallback &cb) {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback &cb) {
                messageCallback_ = cb;
            }

        private:
            /* not thread safe, but in loop */
            void newConnection(int sockfd, const InetAddress &peerAddr);

            //Thread safe
            void removeConnection(const TcpConnectionPtr &conn);

            /* not thread safe, but in loop */
            void removeConnectionInLoop(const TcpConnectionPtr &conn);

            //内部使用Acceptor获取新链接fd，保存客户connectionCallback和messageCallback
            //持有目前存活着的TcpConnection的shared_ptr

            //每一个TcpConnection都有一个名字
            using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

            EventLoop *loop_;                         /* acceptor loop */
            const std::string name_;
            std::unique_ptr<Acceptor> acceptor_;
            std::shared_ptr<EventLoopThreadPool> threadPool_;

            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            ThreadInitCallback threadInitCallback_;
            AtomicInt32 started_;
            int nextConnId_;  // always in loop thread.
            ConnectionMap connections_;
        };
    }
}


#endif //SERVER_TCPSERVER_H
