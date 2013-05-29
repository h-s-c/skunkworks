// Public Domain

#pragma once

#include <cstdint>

#include <EGL/egl.h>

namespace base
{
    class Window
    {
      public:
        Window(std::uint32_t width, std::uint32_t height, bool fullscreen);
        ~Window();
        
        EGLNativeWindowType GetNativeWindow() {return native_window;}
        EGLNativeDisplayType GetNativeDisplay() {return native_display;}
        std::uint32_t GetWidth() { return width;}
        std::uint32_t GetHeight() { return height;}
        
      private:
        std::uint32_t width;
        std::uint32_t height;
        bool fullscreen;
        
        EGLNativeWindowType native_window;
        EGLNativeDisplayType native_display;
    };
}
