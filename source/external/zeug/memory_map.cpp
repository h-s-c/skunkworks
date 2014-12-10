// Public Domain
#include <zeug/memory_map.hpp>
#include <zeug/detail/platform_macros.hpp>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#elif defined(PLATFORM_UNIX)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#endif

namespace zeug
{   
    memory_map::memory_map(const std::string& path)
    {
#if defined(PLATFORM_WINDOWS) 
#elif defined(PLATFORM_UNIX)
        struct stat buf;

        int fd = open(std::string(path).c_str(),O_RDONLY);
        if (fd < 0) 
        {
            std::runtime_error error("Could not map file.");
        }

        if (fstat(fd,&buf) < 0) 
        {
            std::runtime_error error("Unable to determine file size.");
        }
        this->size_internal = (std::size_t)buf.st_size;
        this->file_internal =static_cast<std::uint8_t*>(mmap(0, this->size_internal ,PROT_READ,MAP_FILE|MAP_PRIVATE,fd,0));
#endif
    }
    
    memory_map::~memory_map()
    {
#if defined(PLATFORM_WINDOWS) 
#elif defined(PLATFORM_UNIX)
        munmap(this->file_internal, this->size_internal);
#endif
    }

    std::pair<std::uint8_t*, std::size_t> memory_map::file()
    {
        return std::make_pair(this->file_internal, this->size_internal);
    }
}