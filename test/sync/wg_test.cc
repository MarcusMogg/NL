#include <atomic>

#include "sync/wait_group.h"
#include "thread/thread.h"
#include "gtest/gtest.h"

using namespace NL;
using namespace std;

TEST(WAITGROUP, CNT)
{
    int n = 10;
    WaitGroup w(0);
    int res = 0;
    Mutex m;
    vector<unique_ptr<Thread>> ts;
    for (int i = 0; i < n; i++)
    {
        w.add(1);
        ts.push_back(move(make_unique<Thread>([&res, &w, i, &m]() {
            for (int j = 0; j <= i; j++)
            {
                MutexGuard lock(m);
                res++;
            }
            w.done();
        })));
        ts[i]->start();
    }
    w.wait();
    ASSERT_EQ(res, n * (n + 1) / 2);
}