#pragma once

#include <cstdint>

#include <GLES2/gl2.h>

namespace zeug
{
  namespace opengl
  {
    class buffer
    {
      // API
      public:
        buffer(std::uint32_t type);
        void bind();
        void unbind();

        template<typename T>
        void upload(T data)
        {
            upload_raw(static_cast<const void*>(data));
        }

        // Underlying opengl object
        std::uint32_t native_handle();

      // Internal
      private:
        std::uint32_t native_handle_internal{};
      public:
        buffer(const buffer&) = delete;
        buffer& operator=(const buffer&) = delete;
        ~buffer();
      private:
        void upload_raw(const void* data);
        std::uint32_t type;
    };
  }
}