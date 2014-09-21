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
#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
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

#if defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC) && (COMPILER_VERSION_MAJOR<16)
    template<class T> struct _Unique_if {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T> struct _Unique_if<T[]> {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
        typename _Unique_if<T>::_Single_object
        make_unique(Args&&... args) {
            return unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

    template<class T>
        typename _Unique_if<T>::_Unknown_bound
        make_unique(size_t n) {
            typedef typename remove_extent<T>::type U;
            return unique_ptr<T>(new U[n]());
        }

    template<class T, class... Args>
        typename _Unique_if<T>::_Known_bound
        make_unique(Args&&...) = delete;
#endif
}
