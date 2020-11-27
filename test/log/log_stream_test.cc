#include "gtest/gtest.h"

#include "log/log_stream.h"

using namespace NL;

TEST(LOGSTREAM, Booleans)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), string(""));
    os << true;
    ASSERT_EQ(buf.toString(), string("1"));
    os << '\n';
    ASSERT_EQ(buf.toString(), string("1\n"));
    os << false;
    ASSERT_EQ(buf.toString(), string("1\n0"));
}

TEST(LOGSTREAM, Integers)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), string(""));
    os << 1;
    ASSERT_EQ(buf.toString(), string("1"));
    os << 0;
    ASSERT_EQ(buf.toString(), string("10"));
    os << -1;
    ASSERT_EQ(buf.toString(), string("10-1"));
    os.resetBuffer();

    os << 0 << " " << 123 << 'x' << 0x64;
    ASSERT_EQ(buf.toString(), string("0 123x100"));
}

TEST(LOGSTREAM, IntegerLimits)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    os << -2147483647;
    ASSERT_EQ(buf.toString(), string("-2147483647"));
    os << static_cast<int>(-2147483647 - 1);
    ASSERT_EQ(buf.toString(), string("-2147483647-2147483648"));
    os << ' ';
    os << 2147483647;
    ASSERT_EQ(buf.toString(), string("-2147483647-2147483648 2147483647"));
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::min();
    ASSERT_EQ(buf.toString(), string("-32768"));
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::max();
    ASSERT_EQ(buf.toString(), string("32767"));
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::max();
    ASSERT_EQ(buf.toString(), string("65535"));
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::min();
    ASSERT_EQ(buf.toString(), string("-2147483648"));
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::max();
    ASSERT_EQ(buf.toString(), string("2147483647"));
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::max();
    ASSERT_EQ(buf.toString(), string("4294967295"));
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::min();
    ASSERT_EQ(buf.toString(), string("-9223372036854775808"));
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::max();
    ASSERT_EQ(buf.toString(), string("9223372036854775807"));
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::max();
    ASSERT_EQ(buf.toString(), string("18446744073709551615"));
    os.resetBuffer();

    int16_t a = 0;
    int32_t b = 0;
    int64_t c = 0;
    os << a;
    os << b;
    os << c;
    ASSERT_EQ(buf.toString(), string("000"));
}

TEST(LOGSTREAM, Floats)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();

    os << 0.0;
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << 1.0;
    ASSERT_EQ(buf.toString(), string("1"));
    os.resetBuffer();

    os << 0.1;
    ASSERT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    os << 0.05;
    ASSERT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    os << 0.15;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    double a = 0.1;
    os << a;
    ASSERT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    double b = 0.05;
    os << b;
    ASSERT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    double c = 0.15;
    os << c;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    os << a + b;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    os << 1.23456789;
    ASSERT_EQ(buf.toString(), string("1.23456789"));
    os.resetBuffer();

    os << 1.234567;
    ASSERT_EQ(buf.toString(), string("1.234567"));
    os.resetBuffer();

    os << -123.456;
    ASSERT_EQ(buf.toString(), string("-123.456"));
    os.resetBuffer();
}

TEST(LOGSTREAM, Void)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();

    os << static_cast<void *>(0);
    ASSERT_EQ(buf.toString(), string("0x0"));
    os.resetBuffer();

    os << reinterpret_cast<void *>(8888);
    ASSERT_EQ(buf.toString(), string("0x22B8"));
    os.resetBuffer();
}

TEST(LOGSTREAM, Strings)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();

    os << "Hello ";
    ASSERT_EQ(buf.toString(), string("Hello "));

    string chenshuo = "Shuo Chen";
    os << chenshuo;
    ASSERT_EQ(buf.toString(), string("Hello Shuo Chen"));
}

TEST(LOGSTREAM, Fmts)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();

    os << Fmt("%4d", 1);
    ASSERT_EQ(buf.toString(), string("   1"));
    os.resetBuffer();

    os << Fmt("%4.2f", 1.2);
    ASSERT_EQ(buf.toString(), string("1.20"));
    os.resetBuffer();

    os << Fmt("%4.2f", 1.2) << Fmt("%4d", 43);
    ASSERT_EQ(buf.toString(), string("1.20  43"));
    os.resetBuffer();
}

TEST(LOGSTREAM, Long)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    for (int i = 0; i < 399; ++i)
    {
        os << "123456789 ";
        ASSERT_EQ(buf.length(), 10 * (i + 1));
        ASSERT_EQ(buf.avail(), 4000 - 10 * (i + 1));
    }

    os << "abcdefghi ";
    ASSERT_EQ(buf.length(), 3990);
    ASSERT_EQ(buf.avail(), 10);

    os << "abcdefghi";
    ASSERT_EQ(buf.length(), 3999);
    ASSERT_EQ(buf.avail(), 1);
}
