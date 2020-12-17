#include <functional>

#include "log/logging.h"
#include "net/acceptor.h"
#include "net/event_loop.h"
#include "net/socket_ops.h"
#include "net/tcp_server.h"
using namespace NL;

TcpServer::TcpServer(EventLoop *loop, const Address &addr)
    : loop_(loop),
      name_(addr.toHostPort()),
      acceptor_(new Acceptor(loop, addr)),
      started_(false),
      nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}
TcpServer::~TcpServer()
{
}
void TcpServer::start()
{
    if (!started_)
    {
        started_ = true;
    }

    if (!acceptor_->listenning())
    {
        loop_->runInLoop(
            std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::newConnection(int sockfd, const Address &peerAddr)
{
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_; // always in loop, don't need mutex
    std::string connName = name_ + buf;

    LOG_INFO << "TcpServer::newConnection [" << name_
             << "] - new connection [" << connName
             << "] from " << peerAddr.toHostPort();

    Address localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(
        new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnection [" << name_
             << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    (void)n;
    loop_->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
}