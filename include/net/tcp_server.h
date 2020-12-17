#ifndef _NL_TCP_SERVER_H_
#define _NL_TCP_SERVER_H_
#include <map>
#include <memory>

#include "common/noncopyable.h"
#include "net/address.h"
#include "net/callbacks.h"
#include "net/tcp_connection.h"

namespace NL
{

class Acceptor;
class EventLoop;

class TcpServer : noncopyable
{
private:
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop *loop_; // the acceptor loop
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool started_;
    int nextConnId_; // always in loop thread
    ConnectionMap connections_;

public:
    TcpServer(EventLoop *loop, const Address &addr);
    ~TcpServer();

    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback &cb)
    {
        messageCallback_ = cb;
    }

    void start();

private:
    // should in loop
    void newConnection(int sockfd, const Address &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
};

} // namespace NL

#endif