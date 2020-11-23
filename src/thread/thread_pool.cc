#include "thread/thread_pool.h"

namespace NL
{

ThreadPool::ThreadPool(const string &name)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(name),
      maxQueueSize_(0),
      running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

size_t ThreadPool::queueSize() const
{
    MutexGuard lock(mutex_);
    return queue_.size();
}

void ThreadPool::post(Task f)
{
    if (threads_.empty())
    {
        f();
    }
    else
    {
        MutexGuard lock(mutex_);
        while (isFull() && running_)
        {
            notFull_.wait();
        }
        if (running_)
        {
            assert(!isFull());
            queue_.push_back(std::move(f));
            notEmpty_.notify();
        }
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    threads_.reserve(numThreads);
    running_ = true;
    for (size_t i = 0; i < numThreads; i++)
    {
        threads_.emplace_back(new NL::Thread(
            std::bind(&ThreadPool::runInThread, this),
            name_ + std::to_string(i)));
        threads_[i]->start();
    }
    if (numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
        MutexGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
        notFull_.notifyAll();
    }
    for (auto &thr : threads_)
    {
        thr->join();
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexGuard lock(mutex_);
    while (queue_.empty() && running_)
    {
        notEmpty_.wait();
    }

    Task res;
    if (!queue_.empty())
    {
        res = std::move(queue_.front());
        queue_.pop_front();
        if (maxQueueSize_ > 0)
        {
            notFull_.notify();
        }
    }
    return res;
}

bool ThreadPool::isFull() const
{
    // ==0的时候意味着不设置上限
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try
    {
        if (threadInitCallback_)
        {
            threadInitCallback_();
        }
        while (running_)
        {
            Task task(take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const std::exception &ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}

} // namespace NL