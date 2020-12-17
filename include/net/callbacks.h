#ifndef _NL_CALLBACKS_H_
#define _NL_CALLBACKS_H_
#include <functional>

namespace NL
{
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using TimerCallBack = std::function<void()>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using MessageCallback = std::function<void(const TcpConnectionPtr &,
                                           const char *data,
                                           ssize_t len)>;
} // namespace NL

#endif