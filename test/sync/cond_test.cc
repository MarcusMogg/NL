#include "sync/mutex.h"
#include "thread/thread.h"
#include "gtest/gtest.h"

using namespace NL;
using namespace std;

class FooBar
{
private:
    int n;
    bool flag;
    Mutex m;
    Condition cond;

public:
    FooBar(int n) : flag(false), cond(m)
    {
        this->n = n;
    }

    void foo(function<void()> printFoo)
    {
        MutexGuard lock(m);
        for (int i = 0; i < n; i++)
        {
            while (flag)
            {
                cond.wait();
            }
            printFoo();
            flag = true;
            cond.notify();
        }
    }

    void bar(function<void()> printBar)
    {
        MutexGuard lock(m);
        for (int i = 0; i < n; i++)
        {
            while (!flag)
            {
                cond.wait();
            }
            printBar();
            flag = false;
            cond.notify();
        }
    }
};

shared_ptr<FooBar> foo;
string res;

void testFoo()
{
    foo->foo([]() { res += "foo"; });
}
void testBar()
{
    foo->bar([]() { res += "bar"; });
}

TEST(COND, FOOBAR)
{
    int n = 5;
    foo = make_shared<FooBar>(n);
    res = "";
    unique_ptr<Thread> t1 = make_unique<Thread>(testFoo);
    unique_ptr<Thread> t2 = make_unique<Thread>(testBar);
    t1->start();
    t2->start();
    t1->join();
    t2->join();
    string realres;
    for (size_t i = 0; i < n; i++)
    {
        realres += "foobar";
    }

    ASSERT_EQ(res, realres);
}
