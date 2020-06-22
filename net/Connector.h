//
// Created by 张志龙 on 2020/6/17.
//

#ifndef SERVER_CONNECTOR_H
#define SERVER_CONNECTOR_H

#include "base/noncopyable.h"
#include <functional>

namespace server {
    namespace net {
        class EventLoop;

        class InetAddress;

        class Connector : public noncopyable {
            //只负责与socket链接，不负责创建TcpConnection
        public:
            using NewConnectionCallback = std::function<void(int sockfd)>;

            Connector(EventLoop *loop, const InetAddress &serverAddr);

            ~Connector();

            void setNewConnectionCallback(const NewConnectionCallback &cb) {
                newConnectionCallback_ = cb;
            }

            void start();

            void restart();

            void stop();

        private:
            NewConnectionCallback newConnectionCallback_;
        };
    }
}


#endif //SERVER_CONNECTOR_H
