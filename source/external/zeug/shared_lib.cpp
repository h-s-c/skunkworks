// Public Domain
#include <zeug/shared_lib.hpp>
#include <zeug/platform.hpp>
#include <zeug/detail/platform_macros.hpp>
#include <zeug/detail/util.hpp>

#include <stdexcept>
#include <string>
#include <iostream>

#if defined(PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#elif defined(PLATFORM_UNIX)
#include <dlfcn.h>
#endif

namespace zeug
{   
    shared_lib::shared_lib(const std::string& path)
    {
#if defined(PLATFORM_WINDOWS) 
        auto suffix = ".dll";
        this->shared_lib_internal = static_cast<void*>(LoadLibraryA(std::string(path + name + suffix).c_str()));
        if (!this->shared_lib_internal) 
        {
            auto seperator = path.find_last_of(R"(/)");
            this->shared_lib_internal = static_cast<void*>(LoadLibraryA(path.substr(0,seperator+1) + "lib" + path.substr(seperator) + suffix).c_str()));
            if (!this->shared_lib_internal) 
            {
                std::runtime_error e(zeug::util::win_errstr());
                throw e;
            }
        }
#elif defined(PLATFORM_UNIX)
        #if defined(PLATFORM_MACOSX)
        auto suffix = ".dylib";
        #else
        auto suffix = ".so";
        #endif

        this->shared_lib_internal = dlopen(std::string(path + suffix).c_str(),RTLD_LAZY);
        if (!this->shared_lib_internal) 
        {
            this->shared_lib_internal = dlopen(std::string("lib" + path + suffix).c_str(),RTLD_LAZY);
            if (!this->shared_lib_internal) 
            {
                auto seperator = path.find_last_of("/");
                if(seperator != std::string::npos)
                {
                    this->shared_lib_internal = dlopen(std::string(path.substr(0,seperator+1) + "lib" + path.substr(seperator) + suffix).c_str(),RTLD_LAZY);
                }
                if (!this->shared_lib_internal) 
                {
                    this->shared_lib_internal = dlopen(std::string(zeug::this_app::libdir() + "/" + path + suffix).c_str(),RTLD_LAZY);
                    if (!this->shared_lib_internal) 
                    {
                        this->shared_lib_internal = dlopen(std::string(zeug::this_app::libdir() + "/" + "lib" + path + suffix).c_str(),RTLD_LAZY);
                        if (!this->shared_lib_internal) 
                        {
                             if (!this->shared_lib_internal) 
                             {
                                    std::runtime_error e(dlerror());
                                     throw e;
                            }
                        }
                    }
                }
            }
        }
#endif
    }
    
    shared_lib::~shared_lib()
    {
        if(this->shared_lib_internal)
        {
#if defined(PLATFORM_WINDOWS) 
            FreeLibrary(HMODULE(this->shared_lib_internal));
#elif defined(PLATFORM_UNIX)
            dlclose(this->shared_lib_internal);
#endif
        }
    }
    
    void* shared_lib::symbol(const std::string& name)
    {
#if defined(PLATFORM_WINDOWS) 
        void* sym = GetProcAddress(HMODULE(this->shared_lib_internal),name.c_str());
        if (!sym) 
        {
            std::runtime_error e(zeug::util::win_errstr());
            throw e;
        }
        return sym;
#elif defined(PLATFORM_UNIX)
        void* sym = dlsym(this->shared_lib_internal,name.c_str());
        if (!sym) 
        {
            std::runtime_error e(dlerror());
            throw e;
        }
        return sym;
#endif
    }
}