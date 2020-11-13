#ifndef _NL_TYPES_H_
#define _NL_TYPES_H_

#include <cassert>
#include <cstdint>
#include <string.h> // memset
#include <string>

namespace NL
{

using std::string;

inline void memZero(void *p, size_t n)
{
    memset(p, 0, n);
}

} // namespace NL

#endif