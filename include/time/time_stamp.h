#ifndef _NL_TIME_STAMP_H_
#define _NL_TIME_STAMP_H_

#include <sys/time.h>

#include "common/types.h"

namespace NL
{

class Timestamp
{

public:
    Timestamp() : usSinceEpoch_(0) {}
    explicit Timestamp(int64_t us) : usSinceEpoch_(us) {}

    void swap(Timestamp &rhs)
    {
        std::swap(usSinceEpoch_, rhs.usSinceEpoch_);
    }

    static const int64_t usPerSecond = 1000 * 1000;
    int64_t usSinceEpoch() const { return usSinceEpoch_; }
    time_t secondSinceEpoch() const
    {
        return static_cast<time_t>(usSinceEpoch_ / usPerSecond);
    }

    bool valid() const { return usSinceEpoch_ > 0; }

    string toString() const
    {
        int64_t sec = usSinceEpoch_ / usPerSecond;
        int64_t us = usSinceEpoch_ % usPerSecond;
        return std::to_string(sec) + "." + std::to_string(us);
    }

    string toFormatString() const
    {
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(usSinceEpoch_ / usPerSecond);
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time);

        int us = static_cast<int>(usSinceEpoch_ % usPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 us);
        return buf;
    }

    bool operator<(Timestamp rhs)
    {
        return usSinceEpoch_ < rhs.usSinceEpoch_;
    }
    bool operator==(Timestamp rhs)
    {
        return usSinceEpoch_ == rhs.usSinceEpoch_;
    }

    static Timestamp now()
    {
        timeval ts;
        gettimeofday(&ts, NULL);
        return Timestamp(ts.tv_sec * usPerSecond + ts.tv_usec);
    }

    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int us)
    {
        return Timestamp(static_cast<int64_t>(t) * usPerSecond + us);
    }

    static double timeDiffSecond(Timestamp high, Timestamp low)
    {
        return static_cast<double>(high.usSinceEpoch_ - low.usSinceEpoch_) / usPerSecond;
    }

    static Timestamp addSecond(Timestamp ts, double sec)
    {
        return Timestamp(ts.usSinceEpoch_ + static_cast<int64_t>(sec * usPerSecond));
    }

private:
    int64_t usSinceEpoch_;
};

} // namespace NL
#endif