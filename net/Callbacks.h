//
// Created by 张志龙 on 2020/6/4.
//

#ifndef SERVER_CALLBACKS_H
#define SERVER_CALLBACKS_H

#include "base/Timestamp.h"
#include <functional>
#include <memory>

namespace server {
    using ::std::placeholders::_1;
    using ::std::placeholders::_2;
    using ::std::placeholders::_3;

    template<typename Type>
    inline Type *get_pointer(const std::shared_ptr<Type> &ptr) { return ptr.get(); }

    template<typename Type>
    inline Type *getPointer(const std::unique_ptr<Type> &ptr) { return ptr.get(); }

    namespace net {

        class Buffer;

        class TcpConnection;

        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using TimerCallback = std::function<void()>;
        using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
        using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
        using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
        using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;
        //Timestamp 是poll（2）返回时刻
        using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;

        //data has been read to (buf, len(buf))

        void defaultConnectionCallback(const TcpConnectionPtr &conn);

        void defaultMessageCallback(const TcpConnectionPtr &conn,
                                    Buffer *buffer,
                                    Timestamp receiveTime);

    }    //namespace net
}       //namespace muduo


#endif //SERVER_CALLBACKS_H
