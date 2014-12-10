// Public Domain
#pragma once
#include <zeug/detail/platform_macros.hpp>
#include <zeug/detail/stdfix.hpp>
#include <zeug/shared_lib.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>

#include <EGL/egl.h>

#if defined(PLATFORM_WINDOWS)
#elif defined(PLATFORM_BLACKBERRY)
#elif defined(PLATFORM_ANDROID)
#elif defined(PLATFORM_RASBERRYPI)
#include <bcm_host.h>
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
typedef Atom XAtom;
typedef Display XDisplay;
typedef Screen XScreen;
typedef Status XStatus;
typedef Window XWindow;
#undef Atom
#undef Display
#undef Screen
#undef Status
#undef Window
#undef Expose
#undef Always
#undef Bool
#undef None
#endif

namespace zeug
{
    namespace dynapi
    {
#if defined(PLATFORM_WINDOWS)
#elif defined(PLATFORM_BLACKBERRY)
#elif defined(PLATFORM_ANDROID)
#elif defined(PLATFORM_RASBERRYPI)
        // Dispman
        namespace dispman
        { 
            namespace api
            {
                static std::function<DISPMANX_DISPLAY_HANDLE_T (std::uint32_t)> vc_dispmanx_display_open;
                static std::function<DISPMANX_UPDATE_HANDLE_T (std::uint32_t)> vc_dispmanx_update_start;
                static std::function<DISPMANX_ELEMENT_HANDLE_T (DISPMANX_UPDATE_HANDLE_T, DISPMANX_DISPLAY_HANDLE_T display, int32_t , const VC_RECT_T *, DISPMANX_RESOURCE_HANDLE_T,const VC_RECT_T *, DISPMANX_PROTECTION_T, VC_DISPMANX_ALPHA_T *, DISPMANX_CLAMP_T *, DISPMANX_TRANSFORM_T)> vc_dispmanx_element_add;
                static std::function<std::int32_t (DISPMANX_UPDATE_HANDLE_T)> vc_dispmanx_update_submit_sync;
                static std::function<std::int32_t (DISPMANX_DISPLAY_HANDLE_T, DISPMANX_MODEINFO_T*)> vc_dispmanx_display_get_info;
            }
            namespace detail
            {
                static std::unique_ptr<zeug::shared_lib> lib;
                static std::uint32_t refcnt{};
                static std::mutex mutex{};
            }

            static inline void init()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                if(!detail::lib)
                {
                    detail::lib = std::make_unique<zeug::shared_lib>("bcm_host");
                    api::vc_dispmanx_display_open = detail::lib->function<DISPMANX_DISPLAY_HANDLE_T (std::uint32_t)>("vc_dispmanx_display_open");
                    api::vc_dispmanx_update_start = detail::lib->function<DISPMANX_UPDATE_HANDLE_T (std::uint32_t)>("vc_dispmanx_update_start");
                    api::vc_dispmanx_element_add = detail::lib->function<DISPMANX_ELEMENT_HANDLE_T (DISPMANX_UPDATE_HANDLE_T, DISPMANX_DISPLAY_HANDLE_T display, int32_t , const VC_RECT_T *, DISPMANX_RESOURCE_HANDLE_T,const VC_RECT_T *, DISPMANX_PROTECTION_T, VC_DISPMANX_ALPHA_T *, DISPMANX_CLAMP_T *, DISPMANX_TRANSFORM_T)>("vc_dispmanx_element_add");
                    api::vc_dispmanx_update_submit_sync = detail::lib->function<std::int32_t (DISPMANX_UPDATE_HANDLE_T)>("vc_dispmanx_update_submit_sync");
                    api::vc_dispmanx_display_get_info = detail::lib->function<std::int32_t (DISPMANX_DISPLAY_HANDLE_T, DISPMANX_MODEINFO_T*)>("vc_dispmanx_display_get_info");
                }
                detail::refcnt++;
            }
            static inline void kill()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                detail::refcnt--;
                if(detail::refcnt == 0)
                {
                    detail::lib.release();
                }
            }
        }
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
        // X11
        namespace x11
        { 
            namespace api
            {
                static std::function<XDisplay* (char*)> XOpenDisplay;
                static std::function<int (XDisplay*)> XDefaultScreen;
                static std::function<XScreen* (XDisplay*)> XDefaultScreenOfDisplay;
                static std::function<XWindow (XDisplay*, XWindow, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long)> XCreateSimpleWindow;
                static std::function<XWindow (XDisplay*, int)> XRootWindow;
                static std::function<unsigned long (XDisplay*, int)> XBlackPixel;
                static std::function<unsigned long (XDisplay*, int)> XWhitePixel;
                static std::function<XSizeHints*()> XAllocSizeHints;
                static std::function<XWMHints* ()> XAllocWMHints;
                static std::function<XClassHint* ()> XAllocClassHint;
                static std::function<XStatus (char**, int, XTextProperty*)> XStringListToTextProperty;
                static std::function<int (XDisplay*, XWindow, XTextProperty*, XTextProperty*, char**, int, XSizeHints*, XWMHints*, XClassHint*)> XSetWMProperties;
                static std::function<int (XDisplay*, XWindow, long)> XSelectInput;
                static std::function<int (XDisplay*, XWindow)> XMapWindow;
                static std::function<int (XDisplay*, XWindow)> XDestroyWindow;
                static std::function<int (XDisplay*)> XCloseDisplay;
                static std::function<bool (XDisplay*)> XPending;
                static std::function<int (XDisplay*, XEvent*)> XNextEvent;
                static std::function<int (XDisplay*)> XScreenCount;
                static std::function<int (char*)> XFree;
                static std::function<XAtom (XDisplay*, char*, bool)> XInternAtom;
                static std::function<XStatus (XDisplay*, XWindow, XAtom*, int)> XSetWMProtocols;
                static std::function<int (XDisplay*, XWindow, XAtom, XAtom, int, int, unsigned char*, int)> XChangeProperty;
                static std::function<int (XScreen*)> XWidthOfScreen;
                static std::function<int (XScreen*)> XHeightOfScreen;
                static std::function<int (XDisplay*, XWindow, unsigned long, XSetWindowAttributes*)> XChangeWindowAttributes;
                static std::function<int (XDisplay*)> XFlush;
                static std::function<int (XDisplay*, XWindow, char*)> XStoreName;
            }
            namespace detail
            {
                static std::unique_ptr<zeug::shared_lib> lib;
                static std::uint32_t refcnt{};
                static std::mutex mutex{};
            }

            static inline void init()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                if(!detail::lib)
                {
                    detail::lib = std::make_unique<zeug::shared_lib>("X11");
                    api::XOpenDisplay = detail::lib->function<XDisplay* (char*)>("XOpenDisplay");
                    api::XDefaultScreen = detail::lib->function<int (XDisplay*)>("XDefaultScreen");
                    api::XDefaultScreenOfDisplay = detail::lib->function<XScreen* (XDisplay*)>("XDefaultScreenOfDisplay");
                    api::XCreateSimpleWindow = detail::lib->function<XWindow (XDisplay*, XWindow, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long)>("XCreateSimpleWindow");
                    api::XRootWindow = detail::lib->function<Window (XDisplay*, int)>("XRootWindow");
                    api::XBlackPixel = detail::lib->function<unsigned long (XDisplay*, int)>("XBlackPixel");
                    api::XWhitePixel = detail::lib->function<unsigned long (XDisplay*, int)>("XWhitePixel");
                    api::XAllocSizeHints = detail::lib->function<XSizeHints*()>("XAllocSizeHints");
                    api::XAllocWMHints = detail::lib->function<XWMHints* ()>("XAllocWMHints");
                    api::XAllocClassHint = detail::lib->function<XClassHint* ()>("XAllocClassHint");
                    api::XStringListToTextProperty  = detail::lib->function<XStatus (char**, int, XTextProperty*)>("XStringListToTextProperty");
                    api::XSetWMProperties = detail::lib->function<int (XDisplay*, XWindow, XTextProperty*, XTextProperty*, char**, int, XSizeHints*, XWMHints*, XClassHint*)>("XSetWMProperties");
                    api::XSelectInput = detail::lib->function<int (XDisplay*, XWindow, long)>("XSelectInput");
                    api::XMapWindow = detail::lib->function<int (XDisplay*, XWindow)>("XMapWindow");
                    api::XDestroyWindow = detail::lib->function<int (XDisplay*, XWindow)>("XDestroyWindow");
                    api::XCloseDisplay = detail::lib->function<int (XDisplay*)>("XCloseDisplay");
                    api::XPending = detail::lib->function<bool (XDisplay*)>("XPending");
                    api::XNextEvent = detail::lib->function<int (XDisplay*, XEvent*)>("XNextEvent");
                    api::XScreenCount = detail::lib->function<int (XDisplay*)>("XScreenCount");
                    api::XFree = detail::lib->function<int (char*)>("XFree");
                    api::XInternAtom = detail::lib->function<XAtom (XDisplay*, char*, bool)>("XInternAtom");
                    api::XSetWMProtocols = detail::lib->function<XStatus (XDisplay*, XWindow, XAtom*, int)>("XSetWMProtocols");
                    api::XChangeProperty = detail::lib->function<int (XDisplay*, XWindow, XAtom, XAtom, int, int, unsigned char*, int)>("XChangeProperty");
                    api::XWidthOfScreen = detail::lib->function<int (XScreen*)>("XWidthOfScreen");
                    api::XHeightOfScreen = detail::lib->function<int (XScreen*)>("XHeightOfScreen");
                    api::XChangeWindowAttributes = detail::lib->function<int (XDisplay*, XWindow, unsigned long, XSetWindowAttributes*)>("XChangeWindowAttributes");
                    api::XFlush = detail::lib->function<int (XDisplay*)>("XFlush");
                    api::XStoreName = detail::lib->function<int (XDisplay*, XWindow, char*)>("XStoreName");
                }
                detail::refcnt++;
            }
            static inline void kill()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                detail::refcnt--;
                if(detail::refcnt == 0)
                {
                    detail::lib.release();
                }
            }
        }
#endif
    }
}