#include <cstdint>
#include <stdexcept>
#include <memory>

#include <GLES2/gl2.h>

#include <zeug/opengl/program.hpp>
#include <zeug/opengl/shader.hpp>

namespace zeug
{
  namespace opengl
  {
    program::program()
    {
        this->native_handle_internal = glCreateProgram();
    }

    void program::attach(std::unique_ptr<opengl::shader> shader)
    {
        glAttachShader(this->native_handle_internal, shader.get()->native_handle());
        shaders.push_back(std::move(shader));
    }

    void program::link()
    {
        glLinkProgram(this->native_handle_internal);

        GLint status;
        glGetProgramiv(this->native_handle_internal, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) 
        {
            GLint length = 0;
            glGetProgramiv(this->native_handle_internal, GL_INFO_LOG_LENGTH, &length);
     
            std::vector<char> log(length);
            glGetProgramInfoLog(this->native_handle_internal, length, &length, &log[0]);

            std::string logstr(log.begin(),log.end());
            std::string errormsg = "GL - Failed to link shader.\n" + logstr;
            throw std::runtime_error(errormsg);
        }
    }

    void program::use()
    {
        glUseProgram(this->native_handle_internal);
    }

    std::uint32_t program::native_handle()
    {
        return native_handle_internal;
    }

    program::~program()
    {
        for(auto& shader : this->shaders)
        {
            //glDetachShader(this->native_handle_internal, shader.get()->native_handle());  
        }
        //glDeleteProgram(this->native_handle_internal);
    }
  }
}