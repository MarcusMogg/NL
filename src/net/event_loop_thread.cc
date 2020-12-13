#include "net/event_loop_thread.h"
#include "net/event_loop.h"

using namespace NL;

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      exiting_(false),
      thread_(std::bind(threadFunc, this)),
      mutex_(),
      cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    loop_->quit();
    thread_.join();
}

EventLoop *EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexGuard lock(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait();
        }
    }

    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        MutexGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop_->loop();
}
