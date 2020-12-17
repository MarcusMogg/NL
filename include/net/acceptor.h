#ifndef _NL_ACCEPTOR_H_
#define _NL_ACCEPTOR_H_

#include <functional>

#include "common/noncopyable.h"
#include "net/address.h"
#include "net/channel.h"
#include "net/socket.h"

namespace NL
{
class EventLoop;

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int fd,
                                                     const Address &)>;
    Acceptor(EventLoop *loop, const Address &listenAddr);
    ~Acceptor() {}

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

} // namespace NL

#endif