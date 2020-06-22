//
// Created by 张志龙 on 2020/6/7.
//

#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include <functional>
#include "net/Socket.h"
#include "net/Channel.h"

namespace server {
    namespace net {
        class InetAddress;

        class EventLoop;

        class Acceptor {
            using NewConnectionCallback = std::function<void(int sockfd,
                                                             const InetAddress &)>;
        public:
            explicit Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);

            ~Acceptor();

            void setNewConnectionCallback(const NewConnectionCallback &cb) {
                newConnectionCallback_ = cb;
            }

            bool listening() const { return listening_; }

            void listen();

        private:
            void handleRead();

            EventLoop *loop_;
            Socket acceptSocket_;   //listening socketfd
            Channel acceptChannel_; //observe acceptSocket_ readable event.
            NewConnectionCallback newConnectionCallback_;  //handle readable event.
            bool listening_;
            int idelFd_{};
        };
    }
}


#endif //SERVER_ACCEPTOR_H
