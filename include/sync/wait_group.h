#ifndef _NL_WAIT_GROUP_H_
#define _NL_WAIT_GROUP_H_

#include "common/noncopyable.h"
#include "condition.h"
#include "mutex.h"
namespace NL
{

class WaitGroup : noncopyable
{
public:
    explicit WaitGroup(int count);

    void wait();
    void done();
    // delta 应该大于等于0
    void add(int delta);

private:
    int count_;
    mutable Mutex mutex_;
    Condition cond_;
};

} // namespace NL

#endif