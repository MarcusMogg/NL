#ifndef _NL_THREAD_H_
#define _NL_THREAD_H_

#include "common/noncopyable.h"
#include "common/types.h"
#include "sync/wait_group.h"
#include <atomic>
#include <functional>
namespace NL
{

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, const string &name = string());
    // support std::move()
    // Thread(Thread &&);
    ~Thread();

    void start();
    int join();

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const string &name() const { return name_; }

    static int numCreated() { return numCreated_.load(); }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pid_t tid_;
    pthread_t pthreadId_;
    string name_;
    ThreadFunc func_;
    WaitGroup latch_;

    static std::atomic<int32_t> numCreated_;
};

} // namespace NL
#endif