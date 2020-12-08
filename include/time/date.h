#ifndef _NL_DATE_H_
#define _NL_DATE_H_

#include <time.h>

#include "common/types.h"

namespace NL
{
class Date
{
public:
    Date() : jdn_(0) {}
    explicit Date(int julianDayNum)
        : jdn_(julianDayNum) {}
    Date(int year, int month, int day);
    explicit Date(const struct tm &);

    void swap(Date &rhs) { std::swap(jdn_, rhs.jdn_); }
    bool valid() const { return jdn_ > 0; }

    struct YearMonthDay
    {
        int year;
        int month;
        int day;
    };

    static const int daysPerWeek = 7;
    static const int jdnOf1970_01_01;

    //  yyyy-mm-dd
    string toIsoString() const;

    YearMonthDay yearMonthDay() const;

    int year() const
    {
        return yearMonthDay().year;
    }

    int month() const
    {
        return yearMonthDay().month;
    }

    int day() const
    {
        return yearMonthDay().day;
    }

    //  [0, 1, ..., 6] => [Monday, ..., Saturday,Sunday]
    int weekDay() const
    {
        return jdn_ % daysPerWeek;
    }

    int julianDayNumber() const
    {
        return jdn_;
    }

private:
    int jdn_; // Julian Day Number
};
} // namespace NL

#endif