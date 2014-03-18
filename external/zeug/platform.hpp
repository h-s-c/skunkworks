// Public Domain
#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{
  typedef struct
  {
    std::uint32_t major, minor, patch;
  } version_t;

  namespace platform
  {
    // General properties
    std::string arch();
    std::string name();
    // Print verbose platform information
    std::string verbose();
  }

  namespace this_app
  {
    // General properties
    std::string arch();
    std::string compiler();
    zeug::version_t compiler_version();
    std::string stdlib();
  }

  enum class powerstate {unknown, on_battery, no_battery, charging, charged}; 

  namespace this_battery
  {
    // General properties
    zeug::powerstate state();
    std::pair<bool, std::chrono::seconds> seconds();
    std::pair<bool, std::uint8_t> percent();
  }

  namespace this_cpu
  {
    // General properties
    std::string arch();    
    std::string vendor();
    std::string model();    
    // Cache properties
    std::pair<bool, std::uint32_t> l1_size(); 
    // Endianess
    bool byteorder_little();
    bool byteorder_big();
    // X86 extensions
    bool htt();
    bool sse1();
    bool sse2();
    bool sse3();
    bool ssse3();
    bool sse41();
    bool sse42();
    bool sse4a();
    bool aes();
    bool avx();
    bool avx2();
    bool xop();
    bool fma3();
    bool fma4();
    // ARM extensions
    bool neon();    
  }
  
  namespace this_gpu
  {
    // General properties
    std::string vendor();
    std::string model();
    // Driver properties
    std::string driver_vendor();
    zeug::version_t driver_version();
    // Tell the driver to use a high performance profile
    void driver_perf_profile(bool perf);
  }
}