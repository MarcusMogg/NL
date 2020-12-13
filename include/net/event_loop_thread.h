#ifndef _NL_EVENT_LOOP_THREAD_H_
#define _NL_EVENT_LOOP_THREAD_H_

#include "common/noncopyable.h"
#include "sync/condition.h"
#include "sync/mutex.h"
#include "thread/thread.h"

namespace NL
{

class EventLoop;

class EventLoopThread : noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;
};

} // namespace NL

#endif