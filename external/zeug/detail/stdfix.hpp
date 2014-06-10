// Public Domain
#pragma once
#include <zeug/detail/platform_macros.hpp>

#include <array>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#if defined(COMPILER_MSVC)
#define _ALLOW_KEYWORD_MACROS
#endif

// C++11 workarounds 
#if !defined(thread_local)
#   if defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC) && (COMPILER_VERSION_MAJOR<14)
#       define thread_local __thread
#   elif defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
#       define thread_local __declspec(thread) 
#   endif
#endif

namespace std
{
#if defined(PLATFORM_ANDROID)
  template <class T>
  std::string to_string(T from)
  {
    std::stringstream ss;
    ss << from;
    return ss.str();
  }
#endif
}
