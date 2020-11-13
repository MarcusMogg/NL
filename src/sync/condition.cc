#include "sync/condition.h"
#include <errno.h>

bool NL::Condition::waitForSeconds(double seconds)
{
    timespec cur;
    clock_gettime(CLOCK_REALTIME, &cur);

    const int64_t nsPerSec = static_cast<int64_t>(1e9);

    int64_t nss = static_cast<int64_t>(seconds * nsPerSec);
    cur.tv_sec += static_cast<time_t>((cur.tv_nsec + nss) / nsPerSec);
    cur.tv_nsec = static_cast<long>((cur.tv_nsec + nss) % nsPerSec);

    return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), &cur);
}