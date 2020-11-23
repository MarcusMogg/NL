#include "thread/thread_pool.h"
#include "gtest/gtest.h"

using namespace NL;
using namespace std;

TEST(THREAD_POOL, CNT)
{
    int n = 5000;
    WaitGroup w(0);
    Mutex m;
    int res = 0;
    ThreadPool pool;
    pool.start(8);

    for (int i = 0; i < n; i++)
    {
        w.add(1);
        pool.post([&res, &w, i, &m]() {
            for (int j = 0; j <= i; j++)
            {
                MutexGuard lock(m);
                res++;
            }
            w.done();
        });
    }
    w.wait();
    ASSERT_EQ(res, n * (n + 1) / 2);
}