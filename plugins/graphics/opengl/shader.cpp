#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <GLES2/gl2.h>

#include "plugins/graphics/opengl/shader.hpp"

namespace opengl
{
    shader::shader(const std::string& sourcecode, std::uint32_t type)
    {
        this->native_handle = glCreateShader(type);
        const GLchar *glsource = static_cast<const GLchar *>(sourcecode.c_str());
        glShaderSource(this->native_handle, 1, &glsource, 0);
        glCompileShader(this->native_handle);

        GLint status = GL_FALSE;
        glGetShaderiv(this->native_handle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) 
        {
            GLint length = 0;
            glGetShaderiv(this->native_handle, GL_INFO_LOG_LENGTH, &length);
     
            std::vector<char> log(length);
            glGetShaderInfoLog(this->native_handle, length, &length, &log[0]);

            std::string logstr(log.begin(),log.end());
            std::string errormsg = "GL - Failed to compile shader.\n" + logstr;
            throw std::runtime_error(errormsg);
        }
    }

    shader::~shader()
    {
        glDeleteShader(this->native_handle);
    }
}