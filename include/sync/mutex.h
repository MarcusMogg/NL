#ifndef _NL_MUTEX_H_
#define _NL_MUTEX_H_

#include "common/noncopyable.h"
#include "common/types.h"
#include "thread/current_thread.h"
#include <pthread.h>

__BEGIN_DECLS
extern void __assert_perror_fail(int errnum,
                                 const char *file,
                                 unsigned int line,
                                 const char *function) noexcept __attribute__((__noreturn__));
__END_DECLS

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__); })

namespace NL
{

class Mutex : noncopyable
{
public:
    Mutex() : holder_(0)
    {
        MCHECK(pthread_mutex_init(&mutex_, NULL));
    }
    ~Mutex()
    {
        assert(holder_ == 0);
        MCHECK(pthread_mutex_destroy(&mutex_));
    }

    void lock()
    {
        MCHECK(pthread_mutex_lock(&mutex_));
        //assignHolder();
    }

    void unlock()
    {
        //unassignHolder();
        MCHECK(pthread_mutex_unlock(&mutex_));
    }

private:
    void unassignHolder()
    {
        holder_ = 0;
    }

    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    friend class Condition;
    pthread_mutex_t *getPthreadMutex()
    {
        return &mutex_;
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexGuard
{

public:
    MutexGuard(Mutex &mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexGuard()
    {
        mutex_.unlock();
    }

private:
    Mutex &mutex_;
};

// 为了避免这种情况
// MutexGuard(x);
// 创建了个临时变量立马被销毁了

#define MutexGuard(x) error "Missing guard object name"

} // namespace NL

#endif