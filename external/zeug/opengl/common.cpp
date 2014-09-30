#include <zeug/opengl/common.hpp>

#include <string>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace zeug
{
  namespace opengl
  {
    bool extension(std::string name)
    {
        std::string extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        std::size_t found = extensions.find(name);
        return (found != std::string::npos);
    }
  }
}