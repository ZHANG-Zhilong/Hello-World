// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include "base/noncopyable.h"
#include "base/StringPiece.h"
#include "base/Types.h"
#include "net/Callbacks.h"
#include "net/Buffer.h"
#include "net/InetAddress.h"
#include "Callbacks.h"

#include <memory>

#include <boost/any.hpp>

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

namespace server {
    namespace net {

        class Channel;

        class EventLoop;

        class Socket;

        class TcpConnection : public noncopyable,
                              public std::enable_shared_from_this<TcpConnection> {
            //表示"一次TCP链接🔗", 唯一使用shared_ptr管理的类
            //构造函数的参数是已经建立好链接的socketfd，该类拥有socket，因此析构函数会close（fd）
            //初始状态是kConnected
            //使用Channel获取socket的IO状态
            //自己处理writeable事件
            //readable事件传回给客户，通过MessageCallback---客户是后台
        public:
            TcpConnection(EventLoop *loop, const string &name, int sockfd,
                          const InetAddress &localAddr, const InetAddress &peerAddr);

            ~TcpConnection();

            EventLoop *getLoop() { return loop_; }

            const string &name() const { return name_; }

            const InetAddress &localAddress() const { return localAddr_; }

            const InetAddress &peerAddress() const { return peerAddr_; }

            bool connected() const { return state_ == kConnected; }

            bool disconnected() const { return state_ == kDisconnected; }

            //send
            //void send (const void* message, size_t len);
            void send(const std::string &message);

            void shutdown();

            void setTcpNoDelay(bool on);

            void setConnectionCallback(const ConnectionCallback &cb) {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback &cb) {
                messageCallback_ = cb;
            }

            void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
                writeCompleteCallback_ = cb;
            }

            void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
                highWaterMarkCallback_ = cb;
                highWaterMark_ = highWaterMark;
            }

            // internal use for Tcp-Server, Tcp-Client
            void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

            //called when TcpServer accepts a new connection.
            void connectEstablished();      //should be called only once.
            //called when TcpServer has removed me from its map.
            void connectDestroyed();        //should be called only once.

        private:
            enum StateE {
                kDisconnected, kConnecting, kConnected, kDisconnecting
            };

            const char *stateToString() const;

            void setState(StateE s) { state_ = s; }

            void handleRead(Timestamp receiveTime);

            void handleWrite();

            void handleClose();

            void handleError();

            void sendInLoop(const std::string &message);

            void shutdownInLoop();

            EventLoop *loop_;
            std::string name_;
            StateE state_;
            bool reading_;
            std::shared_ptr<Socket> socket_;
            std::shared_ptr<Channel> channel_;
            InetAddress localAddr_;
            InetAddress peerAddr_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            WriteCompleteCallback writeCompleteCallback_;
            HighWaterMarkCallback highWaterMarkCallback_;
            CloseCallback closeCallback_;
            Buffer inputBuffer_;
            Buffer outputBuffer_;
            size_t highWaterMark_;
        };

        typedef std::shared_ptr<TcpConnection>

                TcpConnectionPtr;

    }  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_TCPCONNECTION_H
