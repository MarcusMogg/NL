#include "net/socket.h"
#include "net/socket_ops.h"

using namespace NL;

Socket::~Socket()
{
    if (sockfd_ >= 0)
    {
        sockets::close(sockfd_);
    }
}

void Socket::bindAddress(const Address &addr)
{
    sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(Address *peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof optval);
    // FIXME CHECK
}
