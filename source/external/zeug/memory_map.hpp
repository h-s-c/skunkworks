// Public Domain
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{
    class memory_map
    {
      // API
      public:
        // Memory maps a file
        memory_map(const std::string& path);
        std::pair<std::uint8_t*, std::size_t> file();
        
      // Internal
      public:
        std::uint8_t* file_internal;
        std::size_t size_internal;
        memory_map(const memory_map&) = delete;
        memory_map& operator=(const memory_map&) = delete;
        ~memory_map();
    };
}