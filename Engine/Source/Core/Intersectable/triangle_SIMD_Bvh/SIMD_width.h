// #define SIMDPP_ARCH_X86_SSE4_1 
// #define SIMDPP_ARCH_X86_SSE2
// #define SIMDPP_ARCH_X86_AVX2
// #define SIMDPP_ARCH_X86_AVX
// #define SIMDPP_ARCH_X86_SSE3
// #define SIMDPP_ARCH_X86_SSSE3
// #define SIMDPP_ARCH_X86_POPCNT_INSN
// #define SIMDPP_ARCH_X86_FMA3
// #define SIMDPP_ARCH_X86_FMA4
// #define SIMDPP_ARCH_X86_XOP
// #define SIMDPP_ARCH_X86_AVX512F
// #define SIMDPP_ARCH_X86_AVX512BW	
// #define SIMDPP_ARCH_X86_AVX512DQ
// #define SIMDPP_ARCH_X86_AVX512VL
// #define SIMDPP_ARCH_ARM_NEON
// #define SIMDPP_ARCH_ARM_NEON_FLT_SP
// #define SIMDPP_ARCH_ARM_NEON
// #define SIMDPP_ARCH_ARM_NEON_FLT_SP
// #define SIMDPP_ARCH_POWER_ALTIVEC
// #define SIMDPP_ARCH_POWER_VSX_206
// #define SIMDPP_ARCH_POWER_VSX_207
// #define SIMDPP_ARCH_MIPS_MSA
#include "simdpp/simd.h"
#include <simdpp/dispatch/get_arch_gcc_builtin_cpu_supports.h>
#include <simdpp/dispatch/get_arch_raw_cpuid.h>
#include <simdpp/dispatch/get_arch_linux_cpuinfo.h>

#if SIMDPP_HAS_GET_ARCH_RAW_CPUID
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_raw_cpuid()
#elif SIMDPP_HAS_GET_ARCH_GCC_BUILTIN_CPU_SUPPORTS
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_gcc_builtin_cpu_supports()
#elif SIMDPP_HAS_GET_ARCH_LINUX_CPUINFO
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_linux_cpuinfo()
#else
#error "Unsupported platform"
#endif


#include <limits>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>
#include "Core/Ray.h"
#include <bitset>
#include "Common/config.h"

void print_arch();




// ifconstexpr