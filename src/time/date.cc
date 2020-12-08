#include "time/date.h"

namespace NL
{

namespace detail
{
int getJulianDayNumber(int year, int month, int day)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153 * m + 2) / 5 + y * 365 + y / 4 - y / 100 + y / 400 - 32045;
}

Date::YearMonthDay getYearMonthDay(int julianDayNumber)
{
    int a = julianDayNumber + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - ((b * 146097) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - ((1461 * d) / 4);
    int m = (5 * e + 2) / 153;
    Date::YearMonthDay ymd;
    ymd.day = e - ((153 * m + 2) / 5) + 1;
    ymd.month = m + 3 - 12 * (m / 10);
    ymd.year = b * 100 + d - 4800 + (m / 10);
    return ymd;
}

} // namespace detail

const int Date::jdnOf1970_01_01 = detail::getJulianDayNumber(1970, 1, 1);

Date::Date(int year, int month, int day)
    : jdn_(detail::getJulianDayNumber(year, month, day)) {}
// tm.tm_year is Year - 1900
Date::Date(const struct tm &val)
    : Date(val.tm_year + 1900, val.tm_mon, val.tm_mday) {}

string Date::toIsoString() const
{
    char buff[16];
    YearMonthDay tmp = yearMonthDay();
    snprintf(buff, sizeof buff, "%4d-%02d-%02d", tmp.year, tmp.month, tmp.day);
    return buff;
}

Date::YearMonthDay Date::yearMonthDay() const
{
    return detail::getYearMonthDay(jdn_);
}

} // namespace NL