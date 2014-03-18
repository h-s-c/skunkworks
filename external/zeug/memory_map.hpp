// Public Domain
#pragma once
#include <memory>
#include <string>
#include <utility>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{
    class memory_map
    {
      // API
      public:
        // Memory maps a file
        memory_map(const std::string& name) : memory_map("", name) {} ;
        memory_map(const std::string& path, const std::string& name);

        // Mapped memory
        std::pair<std::uint8_t*,std::uint32_t> memory;
        
      // Internal
      public:
        memory_map(const memory_map&) = delete;
        memory_map& operator=(const memory_map&) = delete;
        ~memory_map();
    };
}