// Public Domain
#include <zeug/zipreader.hpp>
#include <zeug/memory_map.hpp>
#include <zeug/platform.hpp>
#include <zeug/detail/platform_macros.hpp>
#define MINIZ_HEADER_FILE_ONLY
#include <zeug/thirdparty/miniz.c>

#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

namespace zeug
{   
    zipreader::zipreader(const std::string& path)
    {
        this->archive = std::make_unique<zeug::memory_map>(path);
        std::memset(&zip_archive, 0, sizeof(zip_archive));
        auto result = mz_zip_reader_init_mem(&zip_archive, (const void*)this->archive.get()->file().first, this->archive.get()->file().second, 0);
        if(!result)
        {
            std::runtime_error error("Could not open archive " + path);
           throw error;
        }
    }
    
    zipreader::~zipreader()
    {
        mz_zip_reader_end(&zip_archive);
    }

    std::string zipreader::text_file(const std::string& name)
    {
        auto temp = this->extract(name);
        std::string result = "";
        result.assign((const char*)temp.first, temp.second);
        return result;
    }

    std::pair<std::uint8_t*, std::size_t> zipreader::file(const std::string& name)
    {
        auto temp = this->extract(name);
        return std::make_pair((std::uint8_t*)temp.first, temp.second);
    }

     std::pair<void*, std::size_t> zipreader::extract( const std::string& filename)
    {
        std::size_t size = 0;
        for (auto i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
            {
                std::runtime_error error("mz_zip_reader_file_stat() failed!\n");
                throw error;
            }
            if(!mz_zip_reader_is_file_a_directory(&zip_archive, i))
            {
                if(std::string(file_stat.m_filename) == filename)
                {
                    auto data_ptr = mz_zip_reader_extract_to_heap(&zip_archive, i, &size, 0);
                    if (!data_ptr)
                    {
                        std::runtime_error error("Could not extract file " + filename);
                       throw error;
                    }
                    return std::make_pair(data_ptr, size);
                }
            }
        }
        std::runtime_error error("Could not find file " + filename);
       throw error;
    }
}