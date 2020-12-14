#ifndef _NL_EVENT_LOOP_H_
#define _NL_EVENT_LOOP_H_

#include <memory>
#include <vector>

#include "common/noncopyable.h"
#include "common/types.h"
#include "net/callbacks.h"
#include "sync/mutex.h"
#include "thread/current_thread.h"
#include "time/time_stamp.h"
namespace NL
{
class Channel;
class Poller;
class TimerQueue;
class TimerId;

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            assertInLoopThread();
        }
    }
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    static EventLoop *getEventLoopCurThread();

    void quit();

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    size_t queueSize() const;

    // timers
    /// Safe to call from other threads.
    TimerId runAt(Timestamp time, TimerCallBack cb);
    TimerId runAfter(double delay, TimerCallBack cb);
    TimerId runEvery(double interval, TimerCallBack cb);
    /// Cancels the timer.
    /// Safe to call from other threads.
    void cancel(TimerId timerId);

    void wakeup();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool eventHandling() const { return eventHandling_; }

private:
    void abortNotInLoopThread();
    void handleRead(); // waked up
    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    bool looping_;
    bool quit_;
    bool eventHandling_;          /* atomic */
    bool callingPendingFunctors_; /* atomic */
    int64_t iteration_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    mutable Mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};
} // namespace NL

#endif