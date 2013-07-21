// Public Domain

#include "base/system/window.hpp"
#include "base/platform.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <EGL/egl.h>

#if !defined(USE_SDL2) && defined(PLATFORM_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#elif !defined(USE_SDL2) && defined(PLATFORM_OS_WINDOWS)
#include <Winuser.h>
#elif defined(PLATFORM_OS_ANDROID)
#include <android/native_window.h>
#elif defined(USE_SDL2) || defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS)
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#endif

namespace base
{
    Window::Window(std::uint32_t width, std::uint32_t height, bool fullscreen) : width(width), height(height), fullscreen(fullscreen), native_window(0), native_display(0)
    {
#if !defined(USE_SDL2) && defined(PLATFORM_OS_LINUX)
        int ciScreenNum;
        Screen *cscrScreenPtr;
        XWMHints *xwmhHints;
        XClassHint  *xchClass;
        XTextProperty xtpWinName;
        XSizeHints *xshSize;
        XEvent xeEvent;
         
        this->native_display = XOpenDisplay(NULL);
        if (this->native_display == NULL)
        {//Failed to connect to an X Server
            std::runtime_error e("X11: Could not open display.");
            throw e;
        }
        else
        {
            ciScreenNum = DefaultScreen(this->native_display);
            cscrScreenPtr = DefaultScreenOfDisplay(this->native_display);
        }
         
        this->native_window = XCreateSimpleWindow(this->native_display, RootWindow(this->native_display, ciScreenNum), 0, 0, this->width, this->height, 0, BlackPixel(this->native_display, ciScreenNum), WhitePixel(this->native_display, ciScreenNum));
         
        //Allocate space for the hints
        xshSize = XAllocSizeHints();
        xwmhHints = XAllocWMHints();
        xchClass = XAllocClassHint();
         
        xshSize->flags = PPosition | PSize;
         
        xwmhHints->initial_state = NormalState;
        xwmhHints->input = True;
        xwmhHints->flags = StateHint | InputHint;
        
        char* name = "Skunkworks";
        
        XStringListToTextProperty(&name, 1, &xtpWinName);
         
        xchClass->res_name = name;
        xchClass->res_class = "Base Win";
         
        XSetWMProperties(this->native_display, this->native_window, &xtpWinName, NULL, 0, 0, xshSize, xwmhHints, xchClass);
     
        XSelectInput(this->native_display, this->native_window, ExposureMask | StructureNotifyMask);
        XMapWindow(this->native_display, this->native_window); //Show the window

        
#elif !defined(USE_SDL2) && defined(PLATFORM_OS_WINDOWS)
        this->native_window = (EGLNativeWindowType)CreateWindowEx(0, TEXT("Skunkworks"), NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, this->width, this->height, NULL, NULL, GetModuleHandle(NULL), NULL);
        this->native_display = (EGLNativeDisplayType)GetDC(this->native_window);
        
#elif defined(PLATFORM_OS_ANDROID)
        this->native_display = EGL_DEFAULT_DISPLAY;
        this->native_window = 0;
        
#elif defined(USE_SDL2) || defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS) 
        SDL_Init( SDL_INIT_VIDEO );
        
        this->sdl_window = (void*)SDL_CreateWindow("Skunkworks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, SDL_WINDOW_SHOWN/*|SDL_WINDOW_RESIZABLE*/ );        
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
#if defined(PLATFORM_OS_LINUX) && !defined(USE_SDL2)
        XDestroyWindow(this->native_display, this->native_window);
        XCloseDisplay(this->native_display);
#elif defined(PLATFORM_OS_WINDOWS) && !defined(USE_SDL2) 
        ReleaseDC(this->native_window, this->native_display);
        DestroyWindow(this->native_window);
#elif defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS) || defined(USE_SDL2)
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
    
    bool Window::Closed()
    {
#if defined(PLATFORM_OS_LINUX) && !defined(USE_SDL2)
        while(XPending(this->native_display) > 0)
        {
            XEvent event;
            XNextEvent(this->native_display,&event);
            switch (event.type)
            {
                case  (ConfigureNotify) :
                {
                    XConfigureEvent* cfg=(XConfigureEvent*) &event;
                    this->width = cfg->width;
                    this->height = cfg->height; 
                    break;
                }
                case (DestroyNotify) :
                {
                    return true;
                }
                default:
                {
                    break;
                }
            }
        }
        return false;
#elif defined(PLATFORM_OS_WINDOWS) && !defined(USE_SDL2) 
		return false;
#elif defined(PLATFORM_OS_MACOSX) || defined(PLATFORM_OS_IOS) || defined(USE_SDL2)
		return false;
#endif
    }
}

