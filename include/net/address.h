#ifndef _NL_ADDRESS_H_
#define _NL_ADDRESS_H_

#include <netinet/in.h>

#include "common/types.h"

namespace NL
{

class Address
{
private:
    sockaddr_in addr_;

public:
    explicit Address(uint16_t port);

    // ip should be "1.2.3.4"
    Address(const std::string &ip, uint16_t port);

    Address(const struct sockaddr_in &addr)
        : addr_(addr)
    {
    }

    string toHostPort() const;

    const struct sockaddr_in &getSockAddrInet() const { return addr_; }
    void setSockAddrInet(const struct sockaddr_in &addr) { addr_ = addr; }
};

} // namespace NL

#endif