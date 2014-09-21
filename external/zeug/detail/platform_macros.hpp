// Public Domain
#pragma once

// Detect platform
#if defined(__x86_64__) || defined(_M_X64)
#   define PLATFORM_ARCH_X86_64
#elif defined(__i686__) || defined(_X86_) || defined(_M_IX86)
#   define PLATFORM_ARCH_X86
#elif defined(__aarch64__)
#   define PLATFORM_ARCH_ARM_64
#elif defined(__arm__) || defined(_M_ARM)
#   define PLATFORM_ARCH_ARM
#endif

// Detect other Unix systems
#if defined(__unix__) || defined (unix) || defined(__unix) || defined(_unix)
#   define PLATFORM_UNIX
#endif

// Detect Linux platform
#if defined(linux) || defined(__linux__) || defined(__LINUX__)
#   define PLATFORM_LINUX
#   define PLATFORM_UNIX
#endif

// Detect BSD platform
#if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#   define PLATFORM_BSD
#   define PLATFORM_UNIX
#endif

// Detect Android platform
#if defined(__ANDROID__)
#   define PLATFORM_ANDROID
#   define PLATFORM_UNIX
#endif

// Detect Windows platform
#if (__WINDOWS__)
#define PLATFORM_WINDOWS
#endif

// Detect Mac OS X platform
#if defined(MACOSX) || defined(__MACOSX__)
#   define PLATFORM_MACOSX
#   define PLATFORM_UNIX
#error Platform not yet supported.
#endif

// Detect iOS platform
#if defined(__APPLE__)
#   include <TargetConditionals.h>
#   if defined(TARGET_OS_IPHONE)
#       define PLATFORM_IOS
#       define PLATFORM_UNIX
#   endif
#error Platform not yet supported.
#endif

#if defined(__QNX__ )
#   define PLATFORM_BLACKBERRY
#   define PLATFORM_UNIX
#error Platform not supported (No C++11 support).
#endif

#if defined(__native_client__)
#   define PLATFORM_NACL
#   define PLATFORM_UNIX
#error Platform not supported (Obsolete).
#endif

#if defined(__pnacl__)
#   define PLATFORM_PNACL
#   define PLATFORM_UNIX
#error Platform not supported (No EGL support).
#endif

#if defined(EMSCRIPTEN)
#   define PLATFORM_EMSCRIPTEN
#   define PLATFORM_UNIX
#endif

#if defined(RASBERRYPI)
#   define PLATFORM_RASBERRYPI
#   define PLATFORM_LINUX
#   define PLATFORM_UNIX
#endif

// Detect Clang
#if defined(__clang__) || defined(__llvm__)
#   define COMPILER_CLANG
#   if defined(__GNUC__)
#       define COMPILER_HOST_GCC
#   endif
#   define COMPILER_VERSION_MAJOR __clang_major__
#   define COMPILER_VERSION_MINOR __clang_minor__
#   define COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

// Detect Intel C/C++ Compiler
#if defined(__ICC) || defined(__INTEL_COMPILER)
#   define COMPILER_ICC
#   if defined(__GNUC__)
#       define COMPILER_HOST_GCC
#   elif defined(_MSC_VER)
#       define COMPILER_HOST_MSVC
#   endif
#   if __INTEL_COMPILER_BUILD_DATE > 20140723
#       define COMPILER_VERSION_MAJOR __INTEL_COMPILER / 100
#       define COMPILER_VERSION_MINOR __INTEL_COMPILER % 100
#       define COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#   elif __INTEL_COMPILER_BUILD_DATE == 20140723
#       define COMPILER_VERSION_MAJOR __INTEL_COMPILER / 100
#       define COMPILER_VERSION_MINOR __INTEL_COMPILER % 100
#       define COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#   elif __INTEL_COMPILER_BUILD_DATE == 20131008
#       define COMPILER_VERSION_MAJOR __INTEL_COMPILER / 100
#       define COMPILER_VERSION_MINOR __INTEL_COMPILER % 100
#       define COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20130728
#       define COMPILER_VERSION_MAJOR __INTEL_COMPILER / 100
#       define COMPILER_VERSION_MINOR __INTEL_COMPILER % 100
#       define COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20130607 || __INTEL_COMPILER_BUILD_DATE == 20130606
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 3
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20130514 
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 2
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20130314 || __INTEL_COMPILER_BUILD_DATE == 20130313
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 1
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20130118  || __INTEL_COMPILER_BUILD_DATE == 20130121
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 0
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20121008  || __INTEL_COMPILER_BUILD_DATE == 20121010
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 1
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120731
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120928
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 7
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120821
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 6
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120612
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 5
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120410  || __INTEL_COMPILER_BUILD_DATE == 20120423
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 4
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20120130
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 3
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20111128  || __INTEL_COMPILER_BUILD_DATE == 20111207
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 2
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20111011
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 1
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20110811  || __INTEL_COMPILER_BUILD_DATE == 20110817
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 0
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20110719
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 5
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20110427  || __INTEL_COMPILER_BUILD_DATE == 20110503
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 4
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20110309
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 3
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20110112
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 2
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20101116  || __INTEL_COMPILER_BUILD_DATE == 20101110
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 1
#       error Compiler broken and thereby not supported.
#   elif __INTEL_COMPILER_BUILD_DATE == 20101006
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#       error Compiler broken and thereby not supported.
#    endif
#endif

// Detect Microsoft Visual C/C++
#if defined(_MSC_VER) && !defined(COMPILER_ICC)
#   define COMPILER_MSVC
#   if _MSC_VER == 1800 // Visual Studio 2013
#       define COMPILER_VERSION_MAJOR 12
#       if _MSC_FULL_VER == 180020827 // RC
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       elif _MSC_FULL_VER == 180020617 // Preview
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#           error Compiler broken and thereby not supported.
#        else
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#   elif _MSC_VER == 1700 // Visual Studio 2012
#       define COMPILER_VERSION_MAJOR 11
#       if _MSC_FULL_VER == 170051106 // Update 1 RTM
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 1
#           error Compiler broken and thereby not supported.
#       elif _MSC_FULL_VER == 170051025 // Update 1 CTP 1
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#           error Compiler broken and thereby not supported.
#       elif _MSC_FULL_VER == 170050727 // RTM
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#           error Compiler broken and thereby not supported.
#       elif _MSC_FULL_VER == 170050214 // Beta 1
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#           error Compiler broken and thereby not supported.
#       elif _MSC_FULL_VER == 170040825 // CTP
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#           error Compiler broken and thereby not supported.
#        else
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#   elif _MSC_VER == 1600 // Visual Studio 2010
#       define COMPILER_VERSION_MAJOR 10
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#       error Compiler broken and thereby not supported.
#   endif
#endif

// Detect GNU Compiler Collection
#if defined(__GNUC__) && !defined(COMPILER_ICC) && !defined(COMPILER_CLANG)
#   define COMPILER_GCC
#   define COMPILER_VERSION_MAJOR __GNUC__
#   define COMPILER_VERSION_MINOR __GNUC_MINOR__
#   define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#   if __GNUC__ == 4
#       if __GNUC_MINOR__ <= 6
#           error Compiler broken and thereby not supported.
#       endif
#   endif
#endif

// We need a to include a header to Detect standard libraries
#include <ciso646>

// Detect Dinkumware standard library
#if defined(_CPPLIB_VER) && !defined(__GLIBCXX__) && !defined(__GLIBCPP__)
#   define COMPILER_STDLIB_DINKUMWARE
#   define COMPILER_STDLIB_VERSION _CPPLIB_VER
#endif

// Detect GNU libstdc++ standard library
#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#   define COMPILER_STDLIB_GNU
#   define COMPILER_STDLIB_VERSION __GLIBC__##__GLIBC__MINOR__
#endif

// Detect LLVM libc++ standard library
#if defined(_LIBCPP_VERSION)
#   define COMPILER_STDLIB_LIBCPP
#   define COMPILER_STDLIB_VERSION _LIBCPP_VERSION
#endif

// Cross-platform macros
#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
#   define COMPILER_NOINLINE             __declspec(noinline)
#   define COMPILER_INLINE               __inline
#   define COMPILER_FORCEINLINE          __forceinline
#   define COMPILER_ALIGN(...)           __declspec(align(__VA_ARGS__))
#   define COMPILER_DLLEXPORT            __declspec(dllexport)
#   define COMPILER_USED
#   define COMPILER_STDCALL                 __stdcall
#else
#   define COMPILER_NOINLINE             __attribute__((noinline))
#   define COMPILER_INLINE               inline
#   define COMPILER_FORCEINLINE          inline __attribute__((always_inline))
#   define COMPILER_ALIGN(...)           __attribute__((aligned(__VA_ARGS__)))
#   define COMPILER_DLLEXPORT      
#   define COMPILER_COMPILER_USED        __attribute__((used))   
#   define COMPILER_STDCALL    
#endif