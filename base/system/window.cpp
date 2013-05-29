// Public Domain

#include "base/system/window.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#if !defined(PLATFORM_OS_ANDROID)
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#else
#include <android/native_window.h>
#endif

namespace base
{
    Window::Window(std::uint32_t width, std::uint32_t height, bool fullscreen) : width(width), height(height), fullscreen(fullscreen), native_window(0), native_display(0)
    {
#if !defined(PLATFORM_OS_ANDROID)
        SDL_Init( SDL_INIT_EVERYTHING );
        auto window = SDL_CreateWindow("Skunkworks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN/*|SDL_WINDOW_RESIZABLE*/ );        
        if (!window) 
        {
            std::runtime_error e(SDL_GetError());
            throw e;
        }        

        SDL_SysWMinfo syswm_info;
        SDL_VERSION( &syswm_info.version );
        if (!SDL_GetWindowWMInfo( window, &syswm_info )) 
        {
            std::runtime_error e(SDL_GetError());
            throw e;
        }
    
        std::array<std::string, 6> translation_array {{"Unknown", "Windows", "X11", "DirectFB", "Cocoa", "UIKit"}};
        std::cout << "SDL windowing system: " << translation_array[syswm_info.subsystem] << std::endl; 
       
    #if defined(PLATFORM_OS_WINDOWS)
        this->native_display = (EGLNativeDisplayType)GetDC(syswm_info.info.windows.window);
        this->native_window = (EGLNativeWindowType)syswm_info.info.windows.window;
    #elif defined(PLATFORM_OS_LINUX)
        this->native_display = (EGLNativeDisplayType)syswm_info.info.x11.display;
        this->native_window = (EGLNativeWindowType)syswm_info.info.x11.window;
    #elif defined(PLATFORM_OS_MACOSX)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = (EGLNativeWindowType)syswm_info.info.cocoa.window;
    #elif defined(PLATFORM_OS_IOS)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = (EGLNativeWindowType)syswm_info.info.uikit.window;
    #endif
#else    
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = 0;
#endif
    }

    Window::~Window()
    {
#if !defined(PLATFORM_OS_ANDROID)
        SDL_Quit();
#endif
    }
    
    EGLNativeWindowType Window::GetNativeWindow(EGLint format) 
    {
#if defined(PLATFORM_OS_ANDROID)
        if(!this->native_window)
        {
            ANativeWindow_setBuffersGeometry(this->native_window, this->width, this->height, format);
        }
#endif
        return this->native_window;
    }
}

