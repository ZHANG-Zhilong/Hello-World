// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "net/TcpConnection.h"

#include "base/Logging.h"
#include "base/WeakCallback.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Socket.h"
#include "net/SocketsOps.h"
#include "net/TimerId.h"
#include "net/Buffer.h"

#include <cerrno>

using namespace server;
using namespace server::net;

void server::net::defaultConnectionCallback(const TcpConnectionPtr &conn) {
    LOG_TRACE << conn->localAddress().toIpPort();  // to be continue.
}

void server::net::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp receiveTime) {
    buffer->retrieveAll();
}

//根据read返回值调用 handleWrite、hancleClose、handleError
void TcpConnection::handleRead(Timestamp receiveTime) {
    loop_->assertInLoopThread();
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = saveErrno;
        LOG_SYSERR << "TcpConnection::handleRead()";
        handleError();
    }
}

TcpConnection::TcpConnection(EventLoop *loop, const string &nameArg, int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr)
        : loop_(CHECK_NOTNULL(loop)),
          name_(nameArg),
          state_(kConnected),
          reading_(true),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr),
          highWaterMark_(6 * 1024 * 1024) {
    channel_->setReadCallback([this](auto &&PH1) { handleRead(PH1); });
    channel_->setWriteCallback([this] { handleWrite(); });
    channel_->setCloseCallback([this] { handleClose(); });
    channel_->setErrorCallback([this] { handleError(); });
    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
              << " fd=" << sockfd;
    socket_->setKeepAlive(true);

}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    //是TcpConnection析构前调用的最后一个函数，通知用户链接已经断开
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    channel_->remove();
    //loop_->removeChannel(get_pointer(channel_));
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
              << "fd = " << channel_->fd() << " state = " << stateToString();
    assert(state_ == kDisconnected);
}

const char *TcpConnection::stateToString() const {
    switch (state_) {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnection";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
    }
}

void TcpConnection::handleClose() {
    //调用closeCallback_， 该回调绑定到TcpServer::removeConnection
    loop_->assertInLoopThread();
    LOG_TRACE << "TcpConnection::handleClose state = " << state_;
    assert(state_ == kConnected);
    //we don't close fd, leave it to dtor, so we can find leaks easily.
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    //must be the last line
    closeCallback_(guardThis);
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(channel_->fd());
    LOG_ERROR << " TcpConnection::handleError [" << name_
              << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void TcpConnection::shutdown() {
    //FixMe use compare and swap
    if (state_ == kConnected) {
        //FixMe shared_from_this?
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::send(const string &message) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            loop_->runInLoop(
                    std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const string &message) {
    //先🧵尝试直接发送数据，如果第一次发送完毕，就不会🔛启用WriteCallback；
    //如果只发送了部分数据，则把剩余的数据放入outputBuffer_，并开始关注writeable事件
    //以后在handleWrite中发送剩余的数据
    //如果当前outputBuffer_已经有待发送的数据，就不能先尝试发送，此时发送可能造成数据混乱
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    //if nothing in output queue, try writing directly.
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (implicit_cast<size_t>(nwrote) < message.size()) {
                LOG_TRACE << "I am going to write more data";
            } else if (writeCompleteCallback_) {
                loop_->queueInLoop(
                        std::bind(writeCompleteCallback_, shared_from_this())
                );
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << " TcpConnection:: sendInLoop";
            }
        }
    }
    assert(nwrote > 0);
    if (implicit_cast<size_t>(nwrote) < message.size()) {
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(),
                            outputBuffer_.peek(),
                            outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop(
                            std::bind(writeCompleteCallback_, shared_from_this())
                    );
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            } else {
                LOG_TRACE << " I am going to write more data.";
            }
        } else { //n>=0
            LOG_SYSERR << " TcpConnection::handleWrite()";
        }
    } else {  // !channel_->isWriting()
        LOG_TRACE << "Connection is down, no more writing.";
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket_->setTcpNoDelay(on);
}










