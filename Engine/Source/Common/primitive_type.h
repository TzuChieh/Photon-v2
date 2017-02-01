#pragma once

#include <cstdint>

namespace ph
{

typedef char           int8;
typedef unsigned char  uint8;
typedef int            int32;
typedef unsigned int   uint32;
typedef int64_t        int64;
typedef uint64_t       uint64;
typedef float          float32;
typedef double         float64;

typedef float32 real;
typedef int32   integer;

constexpr inline real operator "" _r(const long double cookedValue)
{
	return static_cast<real>(cookedValue);
}

}// end namespace ph
