#ifndef _NL_POLLER_H_
#define _NL_POLLER_H_

#include <map>
#include <vector>

#include "common/noncopyable.h"
#include "time/time_stamp.h"

struct pollfd;

namespace NL
{

class Channel;
class EventLoop;

class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);
    ~Poller() = default;

    Timestamp poll(int timeOutMs, ChannelList &activeChannels);

    void updateChannel(Channel *ch);

private:
    void fillActiceChannels(int numEvents, ChannelList &activeChannels) const;

    using PollfdList = std::vector<pollfd>;
    using ChannelMap = std::map<int, Channel *>;

    EventLoop *loop_;
    PollfdList pollfds_;
    ChannelMap channels_;
};

} // namespace NL

#endif