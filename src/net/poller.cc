#include <poll.h>

#include "log/logging.h"
#include "net/channel.h"
#include "net/event_loop.h"
#include "net/poller.h"

using namespace NL;

Poller::Poller(EventLoop *loop) : loop_(loop) {}

void Poller::fillActiceChannels(int numEvents, ChannelList &activeChannels) const
{
    for (const auto &i : pollfds_)
    {
        if (i.revents > 0)
        {
            --numEvents;
            auto ch = channels_.find(i.fd);
            assert(ch != channels_.end());
            Channel *channel = ch->second;
            channel->setRevents(i.revents);
            activeChannels.push_back(channel);
        }
    }
}

Timestamp Poller::poll(int timeOutMs, ChannelList &activeChannels)
{
    int numEvnets = ::poll(pollfds_.data(), pollfds_.size(), timeOutMs);

    Timestamp now(Timestamp::now());

    if (numEvnets > 0)
    {
        LOG_TRACE << numEvnets << " events happen";
        fillActiceChannels(numEvnets, activeChannels);
    }
    else if (numEvnets == 0)
    {
        LOG_TRACE << "poll nothing";
    }
    else
    {
        LOG_SYSERR << "Poller::poll()";
    }

    return now;
}

void Poller::updateChannel(Channel *ch)
{
    if (ch->index() < 0)
    {
        assert(channels_.find(ch->fd()) == channels_.end());
        pollfd fd;
        fd.fd = ch->fd();
        fd.events = static_cast<short>(ch->evnets());
        fd.revents = 0;
        ch->setIndex(pollfds_.size());
        pollfds_.push_back(fd);
        channels_[fd.fd] = ch;
    }
    else
    {
        assert(channels_.find(ch->fd()) != channels_.end());
        int idx = ch->index();
        assert(idx >= 0 && idx < pollfds_.size());
        auto &fd = pollfds_[idx];
        assert(fd.fd == ch->fd() || fd.fd == -1);
        fd.events = static_cast<short>(ch->evnets());
        fd.revents = 0;
        if (ch->isNoneEvent())
        {
            fd.fd = -1; // ignore
        }
    }
}