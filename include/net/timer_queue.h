#ifndef _NL_TIMER_QUEUE_H
#define _NL_TIMER_QUEUE_H

#include <atomic>
#include <set>
#include <vector>

#include "common/noncopyable.h"
#include "net/callbacks.h"
#include "net/channel.h"
#include "time/time_stamp.h"

namespace NL
{

class EventLoop;

class Timer
{
public:
    Timer(TimerCallBack cb, Timestamp when, double interval)
        : callback_(std::move(cb)),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.fetch_add(1))
    {
    }

    void run() const
    {
        callback_();
    }

    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(Timestamp now);

    static int64_t numCreated() { return s_numCreated_.load(); }

private:
    const TimerCallBack callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;

    static std::atomic<int64_t> s_numCreated_;
};

class TimerId
{
public:
    TimerId()
        : timer_(nullptr),
          sequence_(0)
    {
    }

    TimerId(Timer *timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    {
    }

    friend class TimerQueue;

private:
    Timer *timer_;
    int64_t sequence_;
};

class TimerQueue : noncopyable
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallBack cb,
                     Timestamp when,
                     double interval);
    void cancel(TimerId id);

private:
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer *, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer *timer);
    void cancelInLoop(TimerId timerId);

    void handleRead();
    // move out all expired timers
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry> &expired, Timestamp now);

    bool insert(Timer *timer);

    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    // Timer list sorted by expiration
    TimerList timers_;

    // for cancel()
    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_; /* atomic */
    ActiveTimerSet cancelingTimers_;
};

} // namespace NL

#endif