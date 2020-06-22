//
// Created by 张志龙 on 2020/6/2.
//

#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include "base/noncopyable.h"
#include "base/Timestamp.h"
#include <functional>
#include <memory>

namespace server {

    namespace net {

        class EventLoop;

        /****************
         * Channel  维护通道，通过fd、events、revents与Poller关联
         * ownerLoop_:EventLoop*   所属的EventLoop
         * fd_, event_, revent_  : int
         * index_:int
         * readCallback, writeCallback, errorCallback: EventCallback(function<void()>)
         * Channel(EventLoop*, int)
         *****************/

        class Channel : noncopyable {
            //断言在事件处理期间，本对象不会析构
        public:
            using EventCallback = std::function<void()>;
            using ReadEventCallabck = std::function<void(Timestamp)>;

            Channel(EventLoop *loop, int fd);

            ~Channel();

            void handleEvent(Timestamp receiveTime);

            void setReadCallback(const ReadEventCallabck &cb) { readCallback_ = cb; }

            void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }

            void setCloseCallback(const EventCallback &cb) { closeCallback_ = cb; }

            void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }

            void tie(const std::shared_ptr<void> &);

            int fd() const { return fd_; }

            int events() const { return events_; }

            void set_revents(int revt) { revents_ = revt; }

            bool isNoneEvent() const { return events_ == kNoneEvent; }

            void enableReading();

            void enableWriting();

            void disableWriting();

            void disableAll();

            void disableReading();;

            bool isWriting() const { return events_ & kWriteEvent; }

            bool isReading() const { return events_ & kReadEvent; }

            int index() const { return index_; }

            void set_index(int idx) { index_ = idx; }

            EventLoop *ownerLoop() { return loop_; }

            void remove();

        private:
            void update();

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;
            EventLoop *loop_;
            const int fd_;              /* File descriptor to poll.  */
            int events_;                /* Types of events poller cares about.  */
            int revents_;               /* Types of events that actually occurred.  */
            int index_;                 /* used by Poller*/
            ReadEventCallabck readCallback_;
            EventCallback writeCallback_;
            EventCallback errorCallback_;
            EventCallback closeCallback_;

            bool tied_{false};
            bool eventHandling_{false};
            std::weak_ptr<void> tie_;
            bool addedToLoop_;

        };
    }
}
#endif //SERVER_CHANNEL_H
