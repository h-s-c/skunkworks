#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include <GLES2/gl2.h>

#include <zeug/opengl/texture.hpp>
#include "thirdparty/stb_image.h"

#include <zeug/platform.hpp>

namespace zeug
{
  namespace opengl
  {
    texture::texture(std::pair<std::uint8_t*,std::uint32_t> file)
    {
        glGenTextures(1,&this->native_handle_internal);
        glBindTexture(GL_TEXTURE_2D,this->native_handle_internal);

        std::int32_t x,y,n;
        auto rawimage = stbi_load_from_memory(file.first, file.second,&x,&y,&n,0);

        if (!rawimage)
        {
            std::string errormsg = "GL - Failed to load image.\n" + std::string(stbi_failure_reason());
            throw std::runtime_error(errormsg);
        }

        int width_in_bytes = x * 4;
        unsigned char *top = NULL;
        unsigned char *bottom = NULL;
        unsigned char temp = 0;
        int half_height = y / 2;

        for (int row = 0; row < half_height; row++) {
          top = rawimage + row * width_in_bytes;
          bottom = rawimage + (y - row - 1) * width_in_bytes;
          for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
          }
        }

        GLenum format = 0;
        if (n==3)
        {
            format=GL_RGB;
        }
        else if (n==4)
        {
            format=GL_RGBA;
        }  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,0,format,x,y,0,format,GL_UNSIGNED_BYTE,rawimage);
        stbi_image_free(rawimage);
    }

    std::uint32_t texture::native_handle()
    {
        return native_handle_internal;
    }

    texture::~texture()
    {
        //glDeleteTextures(1,&this->native_handle_internal);
    }
  }
}