//
// Created by 张志龙 on 2020/6/2.
//

#include "net/Poller.h"
#include "net/Channel.h"
#include "base/Types.h"
#include <algorithm>

#include <poll.h>

using namespace server;
using namespace server::net;

Poller::Poller(EventLoop *eventLoop) :
        ownerLoop_(eventLoop) {
}


Timestamp Poller::poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG_TRACE << numEvents << "events happened";
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_TRACE << "nothing happened";
    } else {
        LOG_SYSERR << "Poller::poll()";
    }
    return now;
}

void Poller::fillActiveChannels(
        int numEvents, Poller::ChannelList *activeChannels) const {
    for (PollFdList::const_iterator pfd = pollfds_.begin();
         pfd != pollfds_.end() && numEvents > 0; ++pfd) {
        if (pfd->revents > 0) {   //revents  当前活动事件🎡
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel *channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }

}

void Poller::updateChannel(Channel *channel) {
    assertInLoopThread();
    LOG_TRACE << "fd = <<" << channel->fd() << " events= " << channel->events();
    if (channel->index() < 0) {  //a new one value is -1 actually.
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd{};
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(channels_.size() - 1);
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else {
        //update existing one
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
        struct pollfd &pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            pfd.events = -1;    //ignore this fd  shelter err event
        }
    }
}

void Poller::assertInLoopThread() {
    ownerLoop_->assertInLoopThread();
}

Poller *Poller::newDefaultPoller(EventLoop *loop) {   //❗️here is different
    return new Poller(loop);
}

void Poller::removeChannel(Channel *channel) {
    Poller::assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    const struct pollfd &pfd = pollfds_[idx];
    (void) pfd;
    assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());  //返回删除元素的数量
    assert(n == 1);
    (void) n;
    if (implicit_cast<size_t>(idx) == pollfds_.size() - 1) {
        pollfds_.pop_back();
    } else {
        int channelAtEnd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
        if (channelAtEnd < 0) {
            channelAtEnd = -channelAtEnd - 1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}

