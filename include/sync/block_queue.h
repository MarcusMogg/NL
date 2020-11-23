#ifndef _NL_BLOCK_QUEUE_H
#define _NL_BLOCK_QUEUE_H

#include <deque>

#include "common/noncopyable.h"
#include "sync/condition.h"
#include "sync/mutex.h"

namespace NL
{

template <typename T>
class BlockQueue : noncopyable
{
public:
    BlockQueue() : mutex_(), cond_(mutex_), queue_() {}

    size_t size() const
    {
        MutexGuard lock(mutex_);
        return queue_.size();
    }

    void push(const T &v)
    {
        MutexGuard lock(mutex_);
        queue_.push_back(v);
        cond_.notify();
    }

    void push(T &&v)
    {
        MutexGuard lock(mutex_);
        queue_.push_back(std::move(v));
        cond_.notify();
    }

    T pop()
    {
        MutexGuard lock(mutex_);
        while (queue_.empty())
        {
            cond_.wait();
        }
        T res(std::move(queue_.front()));
        queue_.pop_front();
        return res;
    }

private:
    mutable Mutex mutex_;
    Condition cond_;
    std::deque<T> queue_;
};

} // namespace NL

#endif