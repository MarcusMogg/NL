#include "net/address.h"
#include "net/socket_ops.h"

using namespace NL;

// It means you can listen any incoming messages.
static const in_addr_t localHost = INADDR_ANY;

Address::Address(uint16_t port)
{
    memZero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET; // ipv4
    addr_.sin_addr.s_addr = sockets::hostToNetwork32(localHost);
    addr_.sin_port = sockets::hostToNetwork16(port);
}

Address::Address(const string &ip, uint16_t port)
{
    memZero(&addr_, sizeof addr_);
    sockets::fromHostPort(ip.c_str(), port, &addr_);
}

string Address::toHostPort() const
{
    char buf[32];
    sockets::toHostPort(buf, sizeof buf, addr_);
    return buf;
}
