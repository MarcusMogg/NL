#include <algorithm>
#include <poll.h>
#include <sys/eventfd.h>

#include "log/logging.h"
#include "net/channel.h"
#include "net/event_loop.h"
#include "net/poller.h"
#include "net/timer_queue.h"

using namespace NL;

thread_local EventLoop *loopInThisThread = nullptr;
const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(true),
      eventHandling_(false),
      callingPendingFunctors_(false),
      iteration_(0),
      threadId_(CurrentThread::tid()),
      poller_(new Poller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(nullptr)
{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if (loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << loopInThisThread
                  << " exists in this thread " << threadId_;
    }
    else
    {
        loopInThisThread = this;
    }
    wakeupChannel_->setReadCallBack(
        std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    assert(!looping_);
    loopInThisThread = nullptr;
}

EventLoop *EventLoop::getEventLoopCurThread()
{
    return loopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, activeChannels_);
        ++iteration_;

        eventHandling_ = true;
        for (auto &i : activeChannels_)
        {
            currentActiveChannel_ = i;
            i->handleEvent();
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;

        doPendingFunctors();
    }

    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        MutexGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

size_t EventLoop::queueSize() const
{
    MutexGuard lock(mutex_);
    return pendingFunctors_.size();
}

TimerId EventLoop::runAt(Timestamp time, TimerCallBack cb)
{
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallBack cb)
{
    Timestamp time(Timestamp::addSecond(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallBack cb)
{
    Timestamp time(Timestamp::addSecond(Timestamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    //timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    //poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return true; // poller_->hasChannel(channel);
}
void EventLoop::wakeup()
{
    uint64_t one = 1;
    /*ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }*/
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    /*ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }*/
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor : functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;
}
