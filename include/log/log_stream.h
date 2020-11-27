#ifndef _NL_LOG_STREAM_H_
#define _NL_LOG_STREAM_H_

#include <cstring>

#include "common/noncopyable.h"
#include "common/string_piece.h"
#include "common/types.h"

namespace NL
{

template <int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer() : len_(0) {}
    ~FixedBuffer() = default;

    int length() const { return len_; }
    int avail() const { return SIZE - len_; }
    const char *data() const { return data_; }
    char *cur() { return data_ + len_; }
    void add(int len) { len_ += len; }

    void append(const char *buf, size_t len)
    {
        if (avail() > len)
        {
            memcpy(data_ + len_, buf, len);
            len_ += len;
        }
    }

    void reset() { len_ = 0; }
    void bzero() { memZero(data_, sizeof data_); }

    string toString() const { return string(data_, length()); }
    StringPiece toStringPiece() const { return StringPiece(data_, length()); }

private:
    int len_;
    char data_[SIZE];
};

class LogStream : noncopyable
{
    using self = LogStream;
    constexpr static const int kMaxNumericSize = 32;
    constexpr static const int kSmallBuffer = 4000;
    constexpr static const int kLargeBuffer = 4000 * 1000;

public:
    using Buffer = FixedBuffer<kSmallBuffer>;

    self &operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self &operator<<(short);
    self &operator<<(unsigned short);
    self &operator<<(int);
    self &operator<<(unsigned int);
    self &operator<<(long);
    self &operator<<(unsigned long);
    self &operator<<(long long);
    self &operator<<(unsigned long long);
    self &operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    self &operator<<(double);

    self &operator<<(const void *);

    self &operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }
    self &operator<<(const char *str)
    {
        if (str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }
    self &operator<<(const string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
    self &operator<<(const StringPiece &v)
    {
        buffer_.append(v.data(), v.size());
        return *this;
    }
    self &operator<<(const Buffer &v)
    {
        *this << v.toStringPiece();
        return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }
    const Buffer &buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    template <typename T>
    void formatInteger(T);

    Buffer buffer_;
};

class Fmt // : noncopyable
{
public:
    template <typename T>
    Fmt(const char *fmt, T val);

    const char *data() const { return buf_; }
    int length() const { return len_; }

private:
    char buf_[32];
    int len_;
};

inline LogStream &operator<<(LogStream &s, const Fmt &fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

} // namespace NL

#endif