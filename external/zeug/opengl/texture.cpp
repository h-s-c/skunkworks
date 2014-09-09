#include <cstdint>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <zeug/opengl/texture.hpp>
#include <zeug/platform.hpp>
#include <zeug/memory_map.hpp>
#include <zeug/detail/platform_macros.hpp>

#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"
#define STB_DXT_IMPLEMENTATION
#include "thirdparty/stb_dxt.h"

// Texture cache (uid, slot , handle)
static std::vector<std::tuple<std::string, std::uint32_t, std::uint32_t, bool>> texture_memcache;

static std::uint32_t slots = -1; 
std::uint32_t get_empty_texture_slot()
{
    slots++;
    return slots;
}

void extract_block(const std::uint8_t*src, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint8_t*block)
{
    if ((w-x >=4) && (h-y >=4))
    {
        // Full Square shortcut
        src += x*4;
        src += y*w*4;
        for (auto i=0; i < 4; ++i)
        {
            *(std::uint32_t*)block = *(std::uint32_t*) src; block += 4; src += 4;
            *(std::uint32_t*)block = *(std::uint32_t*) src; block += 4; src += 4;
            *(std::uint32_t*)block = *(std::uint32_t*) src; block += 4; src += 4;
            *(std::uint32_t*)block = *(std::uint32_t*) src; block += 4; 
            src += (w*4) - 12;
        }
        return;
    }

    std::int32_t bw = std::min(w - x, 4);
    std::int32_t bh = std::min(h - y, 4);
    std::int32_t bx, by;
   
    const std::int32_t rem[] =
    {
        0, 0, 0, 0,
        0, 1, 0, 1,
        0, 1, 2, 0,
        0, 1, 2, 3
    };
     
    for(auto i = 0; i < 4; ++i)
    {
        by = rem[(bh - 1) * 4 + i] + y;
        for(auto j = 0; j < 4; ++j)
        {
            bx = rem[(bw - 1) * 4 + j] + x;
            block[(i * 4 * 4) + (j * 4) + 0] = src[(by * (w * 4)) + (bx * 4) + 0];
            block[(i * 4 * 4) + (j * 4) + 1] = src[(by * (w * 4)) + (bx * 4) + 1];
            block[(i * 4 * 4) + (j * 4) + 2] = src[(by * (w * 4)) + (bx * 4) + 2];
            block[(i * 4 * 4) + (j * 4) + 3] = src[(by * (w * 4)) + (bx * 4) + 3];
        }
    }
}

void compress_tex(std::uint8_t* dst, std::uint8_t* src, std::int32_t w, std::int32_t h, bool alpha )
{
    std::uint8_t block[64];
    for(auto y = 0; y < h; y += 4)
    {
        for(auto x = 0; x < w; x += 4)
        { 
            extract_block(src, x, y, w, h, block);
            stb_compress_dxt_block(dst, block, alpha, STB_DXT_NORMAL);
            #if defined(PLATFORM_ANDROID)
            //convert to atc
            #endif
            dst += alpha ? 16 : 8;
        }
    }
}

bool file_exists(std::string file) 
{
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

namespace zeug
{
  namespace opengl
  {
    texture::texture(std::string uid, std::string filepath, std::string filename, std::pair<std::uint32_t,std::uint32_t> size_xy)
        : size_xy_internal(size_xy)
    {
        bool memcached = false;

        this->uid_internal = uid;

        for(auto texture : texture_memcache)
        {
            if(std::get<0>(texture) == this->uid_internal)
            {
                this->native_slot_internal = std::get<1>(texture);
                this->native_handle_internal = std::get<2>(texture);
                this->ready_internal = std::get<3>(texture);
                memcached = true;
                break;
            }

        }

        if(!memcached)
        {
            this->native_slot_internal = get_empty_texture_slot();
            texture_memcache.push_back(std::make_tuple(uid, this->native_slot_internal, this->native_handle_internal, false));
            
            auto compcache_path = std::string(getenv("HOME")) + std::string("/.cache/") + APP_NAME_STRING;
            mkdir(compcache_path.c_str(), 0700);
            compcache_path = compcache_path + std::string("/textures");
            mkdir(compcache_path.c_str(), 0700);

            this->has_future_internal = true;
            this->future_internal = std::async(std::launch::async, [compcache_path, filepath, filename, size_xy, uid]()
            {
                auto compressed_image_internal = new std::uint8_t[size_xy.first * size_xy.second];
                if (file_exists(compcache_path + "/" + uid))
                {
                    zeug::memory_map file(compcache_path, uid);
                    std::memcpy(compressed_image_internal, file.memory.first, file.memory.second);
                }
                else
                {
                    // Load image
                    zeug::memory_map file(filepath, filename);

                    std::int32_t w,h,n;
                    auto raw_image = stbi_load_from_memory(file.memory.first, file.memory.second, &w, &h, &n, 0);
                    if (!raw_image)
                    {
                        std::string errormsg = "GL - Failed to load image.\n" + std::string(stbi_failure_reason());
                        throw std::runtime_error(errormsg);
                    }

                    // Flip image upside-down
                    {
                        std::int32_t width_in_bytes = w * 4;
                        std::uint8_t *top = nullptr;
                        std::uint8_t *bottom = nullptr;
                        std::uint8_t temp = 0;
                        auto half_height = h / 2;

                        for (auto row = 0; row < half_height; row++) 
                        {
                            top = raw_image + row * width_in_bytes;
                            bottom = raw_image + (h - row - 1) * width_in_bytes;
                            for (auto col = 0; col < width_in_bytes; col++) 
                            {
                                std::swap(*top, *bottom);
                                top++;
                                bottom++;
                            }
                        }
                    }

                    if (n==3)
                    {
                        throw std::runtime_error( "Only RGBA textures supported.\n");
                    }

                    compress_tex( compressed_image_internal , raw_image, w, h, true);

                    std::ofstream compcache_file;
                    compcache_file.open (compcache_path + "/" + uid, std::ios::out | std::ios::binary);
                    if (compcache_file.is_open())
                    {
                        compcache_file.write(reinterpret_cast<char*>(compressed_image_internal), w * h * sizeof(std::uint8_t));
                        compcache_file.close();
                    }
                    else
                    {
                        std::cerr << "Could not cache " << uid << std::endl;
                    }

                    stbi_image_free(raw_image);
                }
                return compressed_image_internal;
            });
        }
    }

    std::uint32_t texture::native_handle()
    {
        return native_handle_internal;
    }

    std::uint32_t texture::native_slot()
    {
        return native_slot_internal;
    }

    bool texture::ready()
    {
        if(!this->ready_internal)
        {
            if(!has_future_internal)
            {
                for(auto texture : texture_memcache)
                {
                    if(std::get<0>(texture) == this->uid_internal)
                    {
                        this->native_slot_internal = std::get<1>(texture);
                        this->native_handle_internal = std::get<2>(texture);
                        this->ready_internal = std::get<3>(texture);
                        break;
                    }

                }
            }
            else if(this->future_internal.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                auto compressed_image = this->future_internal.get();
                glActiveTexture(GL_TEXTURE0 + this->native_slot_internal);
                glGenTextures(1,&this->native_handle_internal);
                glBindTexture(GL_TEXTURE_2D,this->native_handle_internal);
#if defined(PLATFORM_ANDROID)
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ATC_RGB_AMD, size_xy_internal.first, size_xy_internal.second, 0, size_xy_internal.first * size_xy_internal.second * sizeof(unsigned char), compressed_image);
#else
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, size_xy_internal.first, size_xy_internal.second, 0, size_xy_internal.first * size_xy_internal.second * sizeof(unsigned char), compressed_image);
 #endif
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);           

                delete[] compressed_image;   
                this->ready_internal = true;

                for(auto& texture : texture_memcache)
                {
                    if(std::get<0>(texture) == this->uid_internal)
                    {
                        std::get<1>(texture) = this->native_slot_internal;
                        std::get<2>(texture) = this->native_handle_internal;
                        std::get<3>(texture) = this->ready_internal;
                        break;
                    }

                }
            }
        }
        return this->ready_internal;
    }

    texture::~texture()
    {
    }
  }
}