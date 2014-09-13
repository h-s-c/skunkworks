// Public Domain
#pragma once
#include <cstdint>
#include <EGL/egl.h>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{
    class window
    {
      // API
      public:
        // Create borderless fullscreen window
        window() : window(0) {} ;
        window(EGLNativeWindowType window);

        // Query resolution
        std::uint32_t width();
        std::uint32_t height();
        
        // Polls events and returns false if the window closed
        bool poll();

        // Get native window/display
        EGLNativeWindowType native_window(){return native_window(0);} ;
        EGLNativeWindowType native_window(const EGLint format);
        EGLNativeDisplayType native_display();

      // Internal
      private:
        std::uint32_t width_internal{};
        std::uint32_t height_internal{};
        bool fullscreen_internal{};
        
        EGLNativeWindowType native_window_internal{};
        EGLNativeDisplayType native_display_internal{};
        void* native_context_internal{};
      public:
        window(const window&) = delete;
        window& operator=(const window&) = delete;
        ~window();
    };
}