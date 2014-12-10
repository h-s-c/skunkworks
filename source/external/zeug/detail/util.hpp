// Public Domain
#pragma once
#include <zeug/detail/platform_macros.hpp>

#if defined(PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

namespace zeug
{
  namespace util
  {
#if defined(PLATFORM_WINDOWS) 
    // Create a string with last error message
    inline std::string win_errstr()
    {
      DWORD error = GetLastError();
      if (error)
      {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );
        if (bufLen)
        {
          LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
          std::string result(lpMsgStr, lpMsgStr+bufLen);
          
          LocalFree(lpMsgBuf);

          return result;
        }
      }
      return std::string();
    }
#endif
  }
}