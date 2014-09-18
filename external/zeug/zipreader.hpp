// Public Domain
#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <zeug/detail/stdfix.hpp>
#include <zeug/memory_map.hpp>

#define MINIZ_HEADER_FILE_ONLY
#include <zeug/thirdparty/miniz.c>

namespace zeug
{
    class zipreader
    {
      // API
      public:
        zipreader(const std::string& path);
        std::string text_file(const std::string& name);
        std::pair<std::uint8_t*, std::size_t> file(const std::string& name);
        
      // Internal
      private:
        std::pair<void*, std::size_t> extract(const std::string& name);
        std::unique_ptr<zeug::memory_map> archive;
        mz_zip_archive zip_archive;
      public:
        zipreader(const zipreader&) = delete;
        zipreader& operator=(const zipreader&) = delete;
        ~zipreader();
    };
}