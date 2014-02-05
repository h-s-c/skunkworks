#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "plugins/graphics/opengl/shader.hpp"

namespace opengl
{
    class program
    {
      // API
      public:
        void attach(std::unique_ptr<opengl::shader> shader);
        void link();
        void use();
        std::uint32_t native_handle;

      // Internal
      private:
        std::vector<std::unique_ptr<opengl::shader>> shaders;
      public:
        program(const program&) = delete;
        program& operator=(const program&) = delete;        
        program();
        ~program();
    };
}
