#ifndef _NL_NONCOPYABLE_H_
#define _NL_NONCOPYABLE_H_

namespace NL
{

class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    void operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace NL
#endif