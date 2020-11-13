#ifndef _NL_CURRENTTHREAD_H_
#define _NL_CURRENTTHREAD_H_

namespace NL
{
namespace CurrentThread
{

extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char *t_threadName;

void cacheTid();

inline int tid()
{
    if (t_cachedTid == 0)
    {
        cacheTid();
    }
    return t_cachedTid;
}

} // namespace CurrentThread
} // namespace NL

#endif