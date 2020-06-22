//
// Created by 张志龙 on 2020/6/2.
//

#include "net/Channel.h"
#include "base/Logging.h"
#include "net/EventLoop.h"

#include <poll.h>

using namespace server;
using namespace server::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fdArg)
        : loop_(loop), fd_(fdArg), events_(0), revents_(0), index_(-1) {

}

void Channel::handleEvent(Timestamp receiveTime) {   //根据不同事件回调不同函数
    eventHandling_ = true;
    if (revents_ & POLLNVAL) {
        LOG_WARN << "Channel::handle_event() POLLNVal";
    }
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        LOG_WARN << "Channel::handle_event() POLLHUP";
        if (closeCallback_) { closeCallback_(); }
    }
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) { errorCallback_(); }
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) { readCallback_(receiveTime); }
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) { writeCallback_(); }
    }
    eventHandling_ = false;
}

void Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::disableReading() {
    events_ &= ~kReadEvent;
    update();
}

void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

Channel::~Channel() {
    assert(!eventHandling_);
}

