#ifndef _NL_LOG_FILE_H_
#define _NL_LOG_FILE_H_

#include <memory>

#include "common/noncopyable.h"
#include "common/types.h"
#include "sync/mutex.h"

namespace NL
{
namespace FileUtil
{
class AppendFile;
}

class LogFile : noncopyable
{
public:
    LogFile(const string &basename,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);
    ~LogFile() = default;

    void append(const char *logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char *logline, int len);

    static string getLogFileName(const string &basename, time_t *now);

    const string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    std::unique_ptr<Mutex> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<FileUitl::AppendFile> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
};
} // namespace NL

#endif