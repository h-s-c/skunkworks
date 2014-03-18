
#include <zeug/opengl/buffer.hpp>

namespace zeug
{
  namespace opengl
  {
    buffer::buffer(std::uint32_t type)
    {
        glGenBuffers(1, &this->native_handle_internal);
        this->type = type;
    }

    void buffer::bind()
    {
        glBindBuffer(this->type, this->native_handle_internal);
    }

    void buffer::unbind()
    {
        glBindBuffer(this->type, 0);
    }

    void buffer::upload_raw(const void* data)
    {
        glBufferData(this->type, sizeof(data), data, GL_STATIC_DRAW);
    }

    std::uint32_t buffer::native_handle()
    {
        return native_handle_internal;
    }

    buffer::~buffer()
    {
        this->unbind();
        glDeleteBuffers(1, &this->native_handle_internal);
    }
  }
}