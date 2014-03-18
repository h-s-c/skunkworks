#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include <zeug/opengl/shader.hpp>

namespace zeug
{
  namespace opengl
  {
    class program
    {
      // API
      public:
        void attach(std::unique_ptr<zeug::opengl::shader> shader);
        void link();
        void use();
        std::uint32_t native_handle();

      // Internal
      private:
        std::uint32_t native_handle_internal{};
        std::vector<std::unique_ptr<zeug::opengl::shader>> shaders;
      public:
        program(const program&) = delete;
        program& operator=(const program&) = delete;        
        program();
        ~program();
    };
  }
}