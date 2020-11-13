#include "sync/wait_group.h"

NL::WaitGroup::WaitGroup(int count)
    : count_(count),
      mutex_(),
      cond_(mutex_)
{
}

void NL::WaitGroup::wait()
{
    MutexGuard lock(mutex_);
    while (count_ > 0)
    {
        cond_.wait();
    }
}

void NL::WaitGroup::done()
{
    MutexGuard lock(mutex_);
    --count_;
    if (count_ == 0)
    {
        cond_.notifyAll();
    }
}

void NL::WaitGroup::add(int delta)
{
    assert(delta >= 0);
    MutexGuard lock(mutex_);
    count_ += delta;
}