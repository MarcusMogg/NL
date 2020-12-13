#ifndef _NL_CHANNEL_H_
#define _NL_CHANNEL_H_

#include <functional>
#include <memory>

#include "common/noncopyable.h"

namespace NL
{

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallBack = std::function<void()>;

    Channel(EventLoop *loop, int fd);

    void handleEvent();
    void setReadCallBack(const EventCallBack &cb)
    {
        readCallBack_ = cb;
    }
    void setWriteCallBack(const EventCallBack &cb)
    {
        writeCallBack_ = cb;
    }
    void setErrorCallBack(const EventCallBack &cb)
    {
        errorCallBack_ = cb;
    }

    int fd() const { return fd_; }
    int evnets() const { return evnets_; }
    int setRevents(int re) { revnets_ = re; }
    bool isNoneEvent() const { return evnets_ == kNoneEvent; }

    void enableReading()
    {
        evnets_ |= kReadEvent;
        update();
    }
    void enableWriting()
    {
        evnets_ |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        evnets_ &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        evnets_ = kNoneEvent;
        update();
    }

    int index() const { return index_; }
    void setIndex(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

    void remove();

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    int fd_;
    int evnets_;
    int revnets_; // same as pollfd
    int index_;

    EventCallBack readCallBack_;
    EventCallBack writeCallBack_;
    EventCallBack errorCallBack_;
};
} // namespace NL

#endif