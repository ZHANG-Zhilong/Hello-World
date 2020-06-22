//
// Created by 张志龙 on 2020/6/2.
//

#ifndef SERVER_POLLER_H
#define SERVER_POLLER_H

#include "base/noncopyable.h"
#include "base/Timestamp.h"
#include "EventLoop.h"
#include <vector>
#include <map>
#include <poll.h>

namespace server {
    namespace net {


        class Channel;

        /*****************
         * 不拥有Channel对象、只负责IO multiplexing，不负责事件分发
         * pollfds_    vector<pollfd>
         * channels_   pollfd.fd到Channel*的映射
         * ownerLoop_  所归属的EventLoop
         * Poller(EventLoop*)   注册其所属的EventLoop
         * fillActiveChannels(int, ChannelList):void  获取活跃事件序列
         * updateChannels(Channel*):void
         *      维护pollfds_，包括添加和修改，涉及到Channel::fd  Channel::index
         * assertInLoopThread():void
         * *****************/

        class Poller : public noncopyable {

        public:
            typedef std::vector<Channel *> ChannelList;

            explicit Poller(EventLoop *eventLoop);

            ~Poller() = default;

            //分发IO事件，在构造函数中注册的loop中被调用, 获取activeChannels
            Timestamp poll(int timeoutMs, ChannelList *activeChannels);

            //维护和更新pollfds_数组，添加新Channel的复杂度是O(logN)
            void updateChannel(Channel *channel);

            void removeChannel(Channel *channel);

            void assertInLoopThread();

            static Poller *newDefaultPoller(EventLoop *loop);

        private:
            void fillActiveChannels(int numEvents,
                                    ChannelList *activeChannels) const;

            //维护pollfds_数组，以及pollfd.fd到Channel* 的映射channels_
            typedef std::vector<struct pollfd> PollFdList;
            typedef std::map<int, Channel *> ChannelMap; //channel.fd(), Channel*
            EventLoop *ownerLoop_;
            PollFdList pollfds_;
            ChannelMap channels_;                       //每一个channel都是一个链接🔗
        };
    }

}

#endif //SERVER_POLLER_H
