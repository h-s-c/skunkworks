// Public Domain
 
#pragma once
#include "base/platform.hpp"
#include <stdint.h>
#include <stdexcept>
#if defined(COMPILER_MSVC)
#   include <intrin.h>
#endif

namespace base
{
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC))

    uint32_t COMPILER_FORCEINLINE PopCount( uint32_t x )
    {
        return __builtin_popcount(x);
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanReverse( uint32_t x )
    {
        return __builtin_clz(x);
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanForward( uint32_t x )
    {
        return __builtin_ctz(x);
    }
    
    void COMPILER_FORCEINLINE ProcessorFeatures(int out[4], int op) 
    {
        asm volatile ("cpuid" : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3]) : "a"(op)); 
    }

#elif defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
    
    uint32_t COMPILER_FORCEINLINE PopCount( uint32_t x )
    {
        uint32_t r = 0;
        r = __popcnt(x);
        return r;
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanReverse( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanReverse(&r, x);
        return r;
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanForward( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanForward(&r, x);
        return r;
    }
    
    void COMPILER_FORCEINLINE ProcessorFeatures(int out[4], int op) 
    {
        __cpuid(out, op); 
    }

#else
    
    uint32_t COMPILER_FORCEINLINE PopCount( uint32_t x )
    {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanReverse( uint32_t x )
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - PopCount(x);
    }
    
    uint32_t COMPILER_FORCEINLINE BitScanForward(( uint32_t x )
    {
        return PopCount((x & -x) - 1);
    }
#endif

}
