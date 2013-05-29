// Public Domain

#include "base/system/window.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#define USE_SDL2
#ifdef USE_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#endif

namespace base
{
    Window::Window(std::uint32_t width, std::uint32_t height, bool fullscreen) : width(width), height(height), fullscreen(fullscreen), native_window()
    {
    #ifdef USE_SDL2
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
    #else
        SDL_Init( SDL_INIT_VIDEO );
        SDL_Surface* sdl_surface;
        if(this->fullscreen)
        {
            sdl_surface = SDL_SetVideoMode( this->width, this->height, 0, SDL_SWSURFACE | SDL_FULLSCREEN );
        }
        else
        {
            sdl_surface = SDL_SetVideoMode( this->width, this->height, 0, SDL_SWSURFACE);
        }
        
        SDL_SysWMinfo sysInfo;
        SDL_VERSION(&sysInfo.version); 

        if(SDL_GetWMInfo(&sysInfo) <= 0)
        {
            std::runtime_error e(SDL_GetError());
            throw e;
        }
    #endif
    
        std::array<std::string, 6> translation_array {{"Unknown", "Windows", "X11", "DirectFB", "Cocoa", "UIKit"}};
        std::cout << "SDL windowing system: " << translation_array[syswm_info.subsystem] << std::endl; 
       
#if defined(PLATFORM_OS_WINDOWS)
        native_display = (EGLNativeDisplayType)GetDC(syswm_info.info.windows.window);
        native_window = (EGLNativeWindowType)syswm_info.info.windows.window;
#elif defined(PLATFORM_OS_LINUX)
        native_display = (EGLNativeDisplayType)syswm_info.info.x11.display;
        native_window = (EGLNativeWindowType)syswm_info.info.x11.window;
#elif defined(PLATFORM_OS_MACOSX)
        native_display = EGL_DEFAULT_DISPLAY;
        native_window = (EGLNativeWindowType)syswm_info.info.cocoa.window;
#elif defined(PLATFORM_OS_IOS)
        native_display = EGL_DEFAULT_DISPLAY;
        native_window = (EGLNativeWindowType)syswm_info.info.uikit.window;
#endif
    }

    Window::~Window()
    {
        SDL_Quit();
    }
}

