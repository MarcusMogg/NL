#ifndef _NL_TCP_CONNECTION_H_
#define _NL_TCP_CONNECTION_H_

#include <memory>

#include "common/noncopyable.h"
#include "common/types.h"
#include "net/address.h"
#include "net/callbacks.h"

namespace NL
{
class Channel;
class EventLoop;
class Socket;

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
private:
    enum StateE
    {
        kConnecting,
        kConnected,
        kDisconnected,
    };

    EventLoop *loop_;
    string name_;
    StateE state_; // FIXME: use atomic variable
    // we don't expose those classes to client.
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    Address localAddr_;
    Address peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    ConnectionCallback closeCallback_;

    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

public:
    TcpConnection(EventLoop *loop,
                  const std::string &name,
                  int sockfd,
                  const Address &localAddr,
                  const Address &peerAddr);
    ~TcpConnection();

    EventLoop *getLoop() const { return loop_; }
    const std::string &name() const { return name_; }
    const Address &localAddress() { return localAddr_; }
    const Address &peerAddress() { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }

    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectionCallback_ = cb;
    }
    void setCloseCallback(const ConnectionCallback &cb)
    {
        closeCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback &cb)
    {
        messageCallback_ = cb;
    }

    /// Internal use only.

    // called when TcpServer accepts a new connection
    void connectEstablished(); // should be called only once
    void connectDestroyed();
};
} // namespace NL
#endif