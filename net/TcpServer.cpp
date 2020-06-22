//
// Created by 张志龙 on 2020/6/8.
//

#include "TcpServer.h"

#include <utility>
#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/EventLoopThreadPool.h"
#include "net/InetAddress.h"
#include "net/SocketsOps.h"
#include "net/TcpConnection.h"
#include "net/Acceptor.h"

using namespace server;
using namespace server::net;

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    //新链接到达时，Acceptor回调newConnection，后者会创建TcpConnection对象conn，将其加入
    //ConnectionMap，设置好回调，在调用conn->connectEstablished(),该函数回调用户提供的
    //ConnectionCallback

    //单向依赖
    //TcpServer使用Acceptor， 但Acceptor不知道TcpServer的存在
    //TcpServer创建TcpConnection，但TcpConnection不知道TcpServer的存在

    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    {
        LOG_INFO << "TcpServer::newConnection [" << name_
                 << "] - new connection [" << connName
                 << "] from " << peerAddr.toIpPort();
    }
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    //FixMe poll with zero timeout to double confirm the new connection

    EventLoop *ioLoop = threadPool_->getNextLoop();

    TcpConnectionPtr conn(
            new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, _1));  //FixMe  unsafe.
    //让TcpConnection的ConnectionCallback由ioLoop线程调用
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, string nameArg)
        : loop_(CHECK_NOTNULL(loop)),
          name_(std::move(nameArg)),
          acceptor_(new Acceptor(loop, listenAddr, true)),
          threadPool_(new EventLoopThreadPool(loop, name_)),
          connectionCallback_(defaultConnectionCallback),
          messageCallback_(defaultMessageCallback),
          nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
    LOG_FATAL << "TcpServer::~TcpServer [" << name_ << "] destruction";
    for (auto &item:connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(
                std::bind(&TcpConnection::connectDestroyed, conn)
        );
    }
}

void TcpServer::start() {
    if (started_.getAndSet(1) == 0) {
        threadPool_->start(threadInitCallback_);
        assert(!acceptor_->listening());
        loop_->runInLoop(
                std::bind(&Acceptor::listen, getPointer(acceptor_))
        );
    }
}

void TcpServer::setThreadNum(int numThreads) {
    assert(numThreads >= 0);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    loop_->runInLoop([this, conn] { removeConnectionInLoop(conn); });
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    //把conn从ConnectionMap中移出
    loop_->assertInLoopThread();
    {
        LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
                 << "] - connection " << conn->name();
    }
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    //(void) n;
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop([conn] { conn->connectDestroyed(); });
}
