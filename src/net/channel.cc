#include <poll.h>

#include "log/logging.h"
#include "net/channel.h"
#include "net/event_loop.h"
using namespace NL;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      evnets_(kNoneEvent),
      revnets_(kNoneEvent),
      index_(-1)
{
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent()
{
    if (revnets_ & POLLNVAL) //描述符不是一个打开的文件
    {
        LOG_WARN << "Channel::handleEvent POLLNVAL";
    }

    if (revnets_ & (POLLNVAL | POLLERR))
    {
        if (errorCallBack_)
            errorCallBack_();
    }
    // POLLRDHUP判断对端关闭
    if (revnets_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallBack_)
            readCallBack_();
    }
    if (revnets_ & (POLLOUT))
    {
        if (writeCallBack_)
            writeCallBack_();
    }
}