//
// Created by 张志龙 on 2020/6/8.
//
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/SocketsOps.h"
#include <unistd.h>

using namespace server;
using namespace server::net;

void newConnection(int sockfd, const InetAddress &peerAddr) {
    ::write(sockfd, "How are you?\n", 14);
    sockets::close(sockfd);
}

int main() {
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr, true);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();
    return 0;
}
