//
// Created by 张志龙 on 2020/6/9.
//

#include "net/Callbacks.h"
#include "net/TcpConnection.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "net/TcpServer.h"
#include "net/Buffer.h"
#include "base/Timestamp.h"

using namespace server;
using namespace server::net;

void onConnection(const server::net::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->name().c_str(), conn->peerAddress().toIpPort().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const server::net::TcpConnectionPtr &conn,
                server::net::Buffer *buf,
               server::Timestamp receiveTime) {
    printf("onMessage(): received %zd bytes from connection [%s] at %s \n",
           buf->readableBytes(),
           conn->name().c_str(),
           receiveTime.toFormattedString().c_str());
    printf("onMessage(): [%s]\n", buf->retrieveAllAsString().c_str());
}

int main() {
    printf("main(): pid: %d \n", getpid());
    server::net::InetAddress listenAddr(9981);
    server::net::EventLoop loop;

    server::net::TcpServer serv(&loop, listenAddr, "fuck");
    serv.setConnectionCallback(onConnection);
    serv.setMessageCallback(onMessage);
    serv.start();
    loop.loop();
    return 0;
}
