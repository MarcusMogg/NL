#ifndef _NL_SOCKET_H_
#define _NL_SOCKET_H_

#include "common/noncopyable.h"
#include "net/address.h"

namespace NL
{

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {
    }

    Socket(Socket &&rhs)
    {
        sockfd_ = rhs.sockfd_;
        rhs.sockfd_ = -1;
    }
    ~Socket();

    int fd() const { return sockfd_; }

    /// abort if address in use
    void bindAddress(const Address &localaddr);
    /// abort if address in use
    void listen();

    /// On success, returns a non-negative integer that is
    /// a descriptor for the accepted socket, which has been
    /// set to non-blocking and close-on-exec. *peeraddr is assigned.
    /// On error, -1 is returned, and *peeraddr is untouched.
    int accept(Address *peeraddr);

    ///
    /// Enable/disable SO_REUSEADDR
    ///
    void setReuseAddr(bool on);

private:
    int sockfd_;
};

} // namespace NL

#endif