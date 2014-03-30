#pragma once
#include <cstdint>
#include <future>
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
        // Load texture from file (async)
        texture(std::string uid, std::string filepath, std::string filename, std::pair<std::uint32_t,std::uint32_t> size_xy);
        // Underlying opengl objects
        std::uint32_t native_handle();
        std::uint32_t native_slot();

        // Check if texture is ready
        bool ready();

      // Internal
      private:
        std::uint32_t native_handle_internal{};
        std::uint32_t native_slot_internal{};
        std::future<unsigned char*> future_internal;
        bool ready_internal = false;
        bool has_future_internal = false;
        std::pair<std::uint32_t,std::uint32_t> size_xy_internal;
        std::string uid_internal;
      public:
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;
        ~texture();
    };
  }
}