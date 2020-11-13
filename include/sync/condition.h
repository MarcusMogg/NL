#ifndef _NL_CONDITION_H_
#define _NL_CONDITION_H_

#include "mutex.h"
#include <pthread.h>

namespace NL
{

class Condition : noncopyable
{
public:
    Condition(Mutex &mutex) : mutex_(mutex)
    {
        MCHECK(pthread_cond_init(&cond_, nullptr));
    }

    ~Condition()
    {
        MCHECK(pthread_cond_destroy(&cond_));
    }

    void wait()
    {
        MCHECK(pthread_cond_wait(&cond_, mutex_.getPthreadMutex()));
    }

    void notify()
    {
        MCHECK(pthread_cond_signal(&cond_));
    }

    void notifyAll()
    {
        MCHECK(pthread_cond_broadcast(&cond_));
    }

    bool waitForSeconds(double seconds);

private:
    Mutex &mutex_;
    pthread_cond_t cond_;
};

} // namespace NL

#endif