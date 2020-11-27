#include <algorithm>

#include "log/log_stream.h"

namespace NL
{

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";

// Efficient Integer to String Conversions, by Matthew Wilson.
template <typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char *p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10); // i < 0 => lsd < 0
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char *p = buf;

    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}
template <typename T>
void LogStream::formatInteger(T val)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        size_t len = convert(buffer_.cur(), val);
        buffer_.add(len);
    }
}

LogStream &LogStream::operator<<(short val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(unsigned short val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(int val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(unsigned int val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(long val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(long long val)
{
    formatInteger(val);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long long val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(double val)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char *buf = buffer_.cur();
        int len = snprintf(buf, kMaxNumericSize, "%.12g", val);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(const void *val)
{
    uintptr_t ptr = reinterpret_cast<uintptr_t>(val);
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char *buf = buffer_.cur();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, ptr);
        buffer_.add(len + 2);
    }
    return *this;
}
template <typename T>
Fmt::Fmt(const char *fmt, T val)
{
    len_ = snprintf(buf_, 32, fmt, val);
}
// Explicit instantiations

template Fmt::Fmt(const char *fmt, char);

template Fmt::Fmt(const char *fmt, short);
template Fmt::Fmt(const char *fmt, unsigned short);
template Fmt::Fmt(const char *fmt, int);
template Fmt::Fmt(const char *fmt, unsigned int);
template Fmt::Fmt(const char *fmt, long);
template Fmt::Fmt(const char *fmt, unsigned long);
template Fmt::Fmt(const char *fmt, long long);
template Fmt::Fmt(const char *fmt, unsigned long long);

template Fmt::Fmt(const char *fmt, float);
template Fmt::Fmt(const char *fmt, double);

} // namespace NL