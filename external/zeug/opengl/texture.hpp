#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace zeug
{
  namespace opengl
  {
    class texture
    {
      // API
      public:
        // Load texture from file in memory (cachefile/buffer/length of buffer)
        texture(std::string cache_name, std::uint8_t* file, std::uint32_t filesize);
        // Underlying opengl object
        std::uint32_t native_handle();

      // Internal
      private:
        std::uint32_t native_handle_internal{};
      public:
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;
        ~texture();
    };
  }
}