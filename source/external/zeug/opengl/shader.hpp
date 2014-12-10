#pragma once

#include <cstdint>
#include <string>

namespace zeug
{
  namespace opengl
  {
    class shader
    {
      // API
      public:
        // Compile shader
        shader(const std::string& sourcecode, std::uint32_t type);
        // Underlying opengl object
        std::uint32_t native_handle();

      // Internal
      private:
        std::uint32_t native_handle_internal{};
      public:
        shader(const shader&) = delete;
        shader& operator=(const shader&) = delete;
        ~shader();
    };
  }
}