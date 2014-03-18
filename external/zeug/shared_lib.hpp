// Public Domain
#pragma once
#include <cstring>
#include <functional>
#include <string>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{
    class shared_lib
    {
      // API
      public:
        // Loads a shared library
        shared_lib(const std::string& name) : shared_lib("", name) {} ;
        shared_lib(const std::string& path, const std::string& name);

        // Returns address of a symbol from the library
        void* symbol(const std::string& name);

        // Returns function object from the library
        template<typename T>
        std::function<T> function(const std::string& name)
        {
            void* raw_ptr = symbol(name);
            T* func_ptr = nullptr;
            std::memcpy(&func_ptr, &raw_ptr, sizeof(raw_ptr));
            return func_ptr;
        }
        
      // Internal
      private:
        void* shared_lib_internal{};
      public:
        shared_lib(const shared_lib&) = delete;
        shared_lib& operator=(const shared_lib&) = delete;
        ~shared_lib();
    };
}
