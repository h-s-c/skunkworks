#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace opengl
{
    class texture
    {
      // API
      public:
        // Load texture from file in memory
        texture(std::pair<std::uint8_t*,std::uint32_t> file);
        // Underlying opengl object
        std::uint32_t native_handle;

      // Internal
      public:
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;
        ~texture();
    };
}