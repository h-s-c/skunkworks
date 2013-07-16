// Public Domain

#include "base/system/window.hpp"
#include "base/platform.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <EGL/egl.h>

#if !defined(BASE_WINDOW_FORCE_SDL) && defined(PLATFORM_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#elif !defined(BASE_WINDOW_FORCE_SDL) && defined(PLATFORM_OS_WINDOWS)
#include <Winuser.h>
#elif defined(PLATFORM_OS_ANDROID)
#include <android/native_window.h>
#elif defined(BASE_WINDOW_FORCE_SDL) || defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS)
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#endif

namespace base
{
    Window::Window(std::uint32_t width, std::uint32_t height, bool fullscreen) : width(width), height(height), fullscreen(fullscreen), native_window(0), native_display(0)
    {
#if !defined(BASE_WINDOW_FORCE_SDL) && defined(PLATFORM_OS_LINUX)
        if( !(this->native_display = XOpenDisplay(0)) )
        {
            std::runtime_error e("X11: Could not open display.");
            throw e;
        }

        this->native_window = XCreateSimpleWindow(this->native_display, DefaultRootWindow(this->native_display), 0, 0, this->width, this->height, 0, 0, 0);
        XMapWindow(this->native_display, this->native_window);
        XSelectInput(this->native_display, this->native_window, StructureNotifyMask | SubstructureNotifyMask);
        Atom wmProto = XInternAtom(this->native_display, "WM_PROTOCOLS", False);
        Atom wmDelete = XInternAtom(this->native_display, "WM_DELETE_WINDOW", False);
        XChangeProperty(this->native_display, this->native_window, wmProto, XA_ATOM, 32, 0, (const unsigned char*)&wmDelete, 1);
        XEvent evtent;
        do
        {
            XNextEvent(this->native_display, &evtent);
        } while(evtent.type != MapNotify);
        
#elif !defined(BASE_WINDOW_FORCE_SDL) && defined(PLATFORM_OS_WINDOWS)
        this->native_window = (EGLNativeWindowType)CreateWindowEx(0, TEXT("Skunkworks"), NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, this->width, this->height, NULL, NULL, GetModuleHandle(NULL), NULL);
        this->native_display = (EGLNativeDisplayType)GetDC(this->native_window);
        
#elif defined(PLATFORM_OS_ANDROID)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = 0;
        
#elif defined(BASE_WINDOW_FORCE_SDL) || defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS) 
        SDL_Init( SDL_INIT_VIDEO );
        this->sdl_window = (void*)SDL_CreateWindow("Skunkworks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, /*SDL_WINDOW_OPENGL|*/SDL_WINDOW_SHOWN/*|SDL_WINDOW_RESIZABLE*/ );        
        if (!this->sdl_window) 
        {
            std::runtime_error e(SDL_GetError());
            throw e;
        }        

        SDL_SysWMinfo syswm_info;
        SDL_VERSION( &syswm_info.version );
        if (!SDL_GetWindowWMInfo( (SDL_Window*)this->sdl_window, &syswm_info )) 
        {
            std::runtime_error e(SDL_GetError());
            throw e;
        }
        
        std::cout << "SDL version: " << (int)syswm_info.version.major << "." << (int)syswm_info.version.minor << "." << (int)syswm_info.version.patch << "." << std::endl; 
    
        std::array<std::string, 6> translation_array {{"Unknown", "Windows", "X11", "DirectFB", "Cocoa", "UIKit"}};
        std::cout << "SDL windowing system: " << translation_array[syswm_info.subsystem] << std::endl; 
        
    #if defined(PLATFORM_OS_LINUX)
        this->native_display = (EGLNativeDisplayType)syswm_info.info.x11.display;    
        this->native_window = (EGLNativeWindowType)syswm_info.info.x11.window;    
    #elif defined(PLATFORM_OS_WINDOWS)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = (EGLNativeWindowType)syswm_info.info.win.window;       
    #elif defined(PLATFORM_OS_MACOSX)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = (EGLNativeWindowType)syswm_info.info.cocoa.window;
    #elif defined(PLATFORM_OS_IOS)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = (EGLNativeWindowType)syswm_info.info.uikit.window;
    #endif
#endif
    }

    Window::~Window()
    {
#if defined(PLATFORM_OS_LINUX) && !defined(BASE_WINDOW_FORCE_SDL)
        XDestroyWindow(this->native_display, this->native_window);
        XCloseDisplay(this->native_display);
#elif defined(PLATFORM_OS_WINDOWS) && !defined(BASE_WINDOW_FORCE_SDL) 
        ReleaseDC(this->native_window, this->native_display);
        DestroyWindow(this->native_window);
#elif defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS) || defined(BASE_WINDOW_FORCE_SDL)
        SDL_DestroyWindow((SDL_Window*)this->sdl_window); 
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

