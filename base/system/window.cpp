// Public Domain

#include "base/system/window.hpp"

#include <cstdint>
#include <stdexcept>

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

namespace base
{
    Window::Window(std::uint32_t width, std::uint32_t height, bool fullscreen) : width(width), height(height), fullscreen(fullscreen), native_window()
    {
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
        native_window = (EGLNativeWindowType)sysInfo.info.x11.window;
    }

    Window::~Window()
    {
        SDL_Quit();
    }
}

