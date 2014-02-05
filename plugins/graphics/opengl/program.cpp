#include <cstdint>
#include <stdexcept>
#include <memory>

#include <GLES2/gl2.h>

#include "plugins/graphics/opengl/program.hpp"
#include "plugins/graphics/opengl/shader.hpp"

namespace opengl
{
    program::program()
    {
        this->native_handle = glCreateProgram();
    }

    void program::attach(std::unique_ptr<opengl::shader> shader)
    {
        glAttachShader(this->native_handle, shader.get()->native_handle);
        shaders.push_back(std::move(shader));
    }

    void program::link()
    {
        glLinkProgram(this->native_handle);

        GLint status;
        glGetProgramiv(this->native_handle, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) 
        {
            GLint length = 0;
            glGetProgramiv(this->native_handle, GL_INFO_LOG_LENGTH, &length);
     
            std::vector<char> log(length);
            glGetProgramInfoLog(this->native_handle, length, &length, &log[0]);

            std::string logstr(log.begin(),log.end());
            std::string errormsg = "GL - Failed to link shader.\n" + logstr;
            throw std::runtime_error(errormsg);
        }
    }

    void program::use()
    {
        glUseProgram(this->native_handle);
    }

    program::~program()
    {
        glDeleteProgram(this->native_handle);
    }
}