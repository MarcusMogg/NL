#ifndef _NL_THREAD_POOL_H_
#define _NL_THREAD_POOL_H_

#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include "common/noncopyable.h"
#include "common/types.h"
#include "sync/condition.h"
#include "sync/mutex.h"
#include "thread/thread.h"

namespace NL
{

class ThreadPool : noncopyable
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const string &name = "ThreadPool");
    ~ThreadPool();

    // 在调用start之前设置
    void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
    void setThreadInitCallback(const Task &cb) { threadInitCallback_ = cb; }
    const string &name() const { return name_; }

    size_t queueSize() const;

    void post(Task f);
    void start(int numThreads);
    void stop();

private:
    Task take();
    // 加锁后调用
    bool isFull() const;
    void runInThread();

    mutable Mutex mutex_;
    Condition notEmpty_;
    Condition notFull_;

    string name_;
    Task threadInitCallback_; // Q: 为什么需要这个函数
    std::vector<std::unique_ptr<Thread>> threads_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;

    bool running_;
};

} // namespace NL

#endif
