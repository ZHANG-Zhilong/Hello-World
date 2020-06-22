//
// Created by 张志龙 on 2020/5/31.
//

#ifndef SERVER_NONCOPYABLE_H
#define SERVER_NONCOPYABLE_H

namespace server
{

    class noncopyable
    {
    public:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

}  // namespace muduo
#endif //SERVER_NONCOPYABLE_H
