#include "thread/current_thread.h"
namespace NL
{
namespace CurrentThread
{

// 全局变量不能声明在头文件，会产生重复定义
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char *t_threadName = "unknown";

} // namespace CurrentThread
} // namespace NL