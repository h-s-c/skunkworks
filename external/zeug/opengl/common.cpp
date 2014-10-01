#include <zeug/opengl/common.hpp>

#include <mutex>
#include <string>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace zeug
{
  namespace opengl
  {
    namespace detail
    {
        static std::string extensions;
        static std::once_flag check_extensions_flag;

    }
    bool extension(const std::string& name)
    {
        std::call_once(detail::check_extensions_flag, []() { detail::extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)); });
        std::size_t found = detail::extensions.find(name);
        return (found != std::string::npos);
    }
  }
}