#include "thread/thread.h"
#include "thread/current_thread.h"

#include <errno.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace NL
{

namespace detail
{

// task id
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer nameInit;

struct ThreadData
{
    using ThreadFunc = Thread::ThreadFunc;

    pid_t *tid_;
    string name_;
    ThreadFunc func_;
    WaitGroup *latch_;

    ThreadData(ThreadFunc func,
               const string &name,
               pid_t *tid,
               WaitGroup *latch)
        : func_(std::move(func)),
          name_(name),
          tid_(tid),
          latch_(latch)
    {
    }

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->done();   // 不判断为null？
        latch_ = nullptr; // 这里有必要吗？

        CurrentThread::t_threadName = name_.empty() ? "NyThread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName); // 设置进程名
        try
        {
            func_();
            CurrentThread::t_threadName = "finished";
        }
        catch (const std::exception &ex)
        {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw; // rethrow
        }
    }
};

void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

} // namespace detail

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

std::atomic<int32_t> Thread::numCreated_;

Thread::Thread(ThreadFunc func, const string &n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name_(n),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.fetch_add(1);
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    detail::ThreadData *data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    // 线程标识符,线程属性,线程函数地址,函数参数
    if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
    {
        started_ = false;
        delete data; // or no delete?
        // LOG_SYSFATAL << "Failed in pthread_create";
    }
    else
    {
        latch_.wait(); // 这里为什么要阻塞？等待线程真正开始？
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

} // namespace NL