#ifndef _NL_TIME_ZONE_H_
#define _NL_TIME_ZONE_H_

#include <memory>
#include <time.h>

namespace NL
{
class TimeZone
{
public:
    // LoadLocation
    explicit TimeZone(const char *zonefile);
    // FixedZone
    TimeZone(int eastOfUtc, const char *tzname);
    TimeZone() = default;

    bool valid() const
    {
        // operator bool () 提供一个本类型到bool的隐式转换
        // 'explicit operator bool() const' in C++11
        return static_cast<bool>(data_);
    }

    // not static function, depend on time zone
    tm toLocalTime(time_t secondSinceEpoch) const;
    time_t fromLocalTime(const tm &) const;

    // gmtime(3)
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    // timegm(3)
    static time_t fromUtcTime(const struct tm &);
    // year in [1900..2500], month in [1..12], day in [1..31]
    static time_t fromUtcTime(int year, int month, int day,
                              int hour, int minute, int seconds);

    struct Data;

private:
    std::shared_ptr<Data> data_;
};
} // namespace NL

#endif