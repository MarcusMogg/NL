#include "sync/mutex.h"
#include "thread/thread.h"
#include "gtest/gtest.h"

using namespace NL;
using namespace std;
Mutex cntlock;
int cnt = 0;

void testFunc1()
{
    for (size_t i = 0; i < 10; i++)
    {
        MutexGuard lock(cntlock);
        cnt++;
        usleep(10000);
    }
}

TEST(MUTEX, CNT)
{
    vector<unique_ptr<Thread>> ts;
    for (size_t i = 0; i < 16; i++)
    {
        ts.push_back(move(make_unique<Thread>(&testFunc1)));
        ts[i]->start();
    }
    for (size_t i = 0; i < 16; i++)
    {
        ts[i]->join();
    }
    ASSERT_EQ(cnt, 16 * 10);
}
