// ======================================================================== //
// Copyright 2009-2012 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "base/system/info.hpp"
#include "base/system/intrinsics.hpp"
#include "base/platform.hpp"

#include <string>

////////////////////////////////////////////////////////////////////////////////
/// All Platforms
////////////////////////////////////////////////////////////////////////////////

namespace base
{
  std::string GetPlatformName() 
  {
#if defined(PLATFORM_OS_LINUX) && !defined(PLATFORM_ARCH_X86_64)
    return "Linux (32bit)";
#elif defined(PLATFORM_OS_LINUX) && defined(PLATFORM_ARCH_X86_64)
    return "Linux (64bit)";
#elif defined(PLATFORM_OS_FREEBSD) && !defined(PLATFORM_ARCH_X86_64)
    return "FreeBSD (32bit)";
#elif defined(PLATFORM_OS_FREEBSD) && defined(PLATFORM_ARCH_X86_64)
    return "FreeBSD (64bit)";
#elif defined(PLATFORM_OS_CYGWIN) && !defined(PLATFORM_ARCH_X86_64)
    return "Cygwin (32bit)";
#elif defined(PLATFORM_OS_CYGWIN) && defined(PLATFORM_ARCH_X86_64)
    return "Cygwin (64bit)";
#elif defined(PLATFORM_OS_WINDOWS) && !defined(PLATFORM_ARCH_X86_64)
    return "Windows (32bit)";
#elif defined(PLATFORM_OS_WINDOWS) && defined(PLATFORM_ARCH_X86_64)
    return "Windows (64bit)";
#elif defined(PLATFORM_OS_MACOSX) && !defined(PLATFORM_ARCH_X86_64)
    return "MacOS (32bit)";
#elif defined(PLATFORM_OS_MACOSX) && defined(PLATFORM_ARCH_X86_64)
    return "MacOS (64bit)";
#elif defined(PLATFORM_OS_UNIX) && !defined(PLATFORM_ARCH_X86_64)
    return "Unix (32bit)";
#elif defined(PLATFORM_OS_UNIX) && defined(PLATFORM_ARCH_X86_64)
    return "Unix (64bit)";
#else
    return "Unknown";
#endif
  }

  std::string GetCPUVendor()
  {
    int cpuinfo[4]; 
    ProcessorFeatures(cpuinfo, 0); 
    int name[4];
    name[0] = cpuinfo[1];
    name[1] = cpuinfo[3];
    name[2] = cpuinfo[2];
    name[3] = 0;
    return (char*)name;
  }

  CPUModel GetCPUModel() 
  {
    int out[4];
    ProcessorFeatures(out, 0);
    if (out[0] < 1) return CPU_UNKNOWN;
    ProcessorFeatures(out, 1);
    int family = ((out[0] >> 8) & 0x0F) + ((out[0] >> 20) & 0xFF);
    int model  = ((out[0] >> 4) & 0x0F) | ((out[0] >> 12) & 0xF0);
    if (family !=   6) return CPU_UNKNOWN;           // earlier than P6
    if (model == 0x0E) return CPU_CORE1;             // Core 1
    if (model == 0x0F) return CPU_CORE2;             // Core 2, 65 nm
    if (model == 0x16) return CPU_CORE2;             // Core 2, 65 nm Celeron
    if (model == 0x17) return CPU_CORE2;             // Core 2, 45 nm
    if (model == 0x1A) return CPU_CORE_NEHALEM;      // Core i7, Nehalem
    if (model == 0x1E) return CPU_CORE_NEHALEM;      // Core i7
    if (model == 0x1F) return CPU_CORE_NEHALEM;      // Core i7
    if (model == 0x2C) return CPU_CORE_NEHALEM;      // Core i7, Xeon
    if (model == 0x2E) return CPU_CORE_NEHALEM;      // Core i7, Xeon
    if (model == 0x2A) return CPU_CORE_SANDYBRIDGE;  // Core i7, SandyBridge
    return CPU_UNKNOWN;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>

namespace base
{
  std::string GetExecutableFilePath() {
    char filename[1024];
    if (GetModuleFileName(NULL, filename, sizeof(filename))) 
    {
        PathRemoveFileSpec(filename);
    }
    return std::string(filename);
  }

  size_t GetNumberOfLogicalThreads() 
  {
#if (_WIN32_WINNT >= 0x0601)
    int groups = GetActiveProcessorGroupCount();
    int totalProcessors = 0;
    for (int i = 0; i < groups; i++) 
      totalProcessors += GetActiveProcessorCount(i);
    return totalProcessors;
#else
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#endif
  }

  int GetTerminalWidth() 
  {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) return 80;
    CONSOLE_SCREEN_BUFFER_INFO info = { 0 };
    GetConsoleScreenBufferInfo(handle, &info);
    return info.dwSize.X;
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Linux Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_LINUX)

#include <stdio.h>
#include <unistd.h>

namespace base
{
  std::string GetExecutableFilePath() 
  {
    char pid[32]; sprintf(pid, "/proc/%d/exe", getpid());
    char buf[1024];
    std::string path = "";
    int bytes = readlink(pid, buf, sizeof(buf)-1);
    if (bytes != -1) 
    {
        buf[bytes] = '\0';
        path = std::string(buf);
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return path + std::string("/");
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// MacOS Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_MACOSX)

#include <mach-o/dyld.h>

namespace base
{
  std::string GetExecutableFilePath()
  {
    char buf[1024];
    std::string path = "";
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) 
    {
        path = std::string(buf);
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return std::string(buf);
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_UNIX)

#include <unistd.h>
#include <sys/ioctl.h>


namespace base
{
  size_t GetNumberOfLogicalThreads() {
    return sysconf(_SC_NPROCESSORS_CONF);
  }

  int GetTerminalWidth() 
  {
    struct winsize info;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &info) < 0) return 80;
    return info.ws_col;
  }
}
#endif

