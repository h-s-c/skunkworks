// Public Domain
#include <zeug/window.hpp>
#include <zeug/detail/platform_macros.hpp>
#include <zeug/detail/dynapi.hpp>
#include <zeug/detail/stdfix.hpp>
#include <zeug/detail/util.hpp>
#include <zeug/shared_lib.hpp>

#include <cstdint>
#include <mutex>
#include <stdexcept>

#include <EGL/egl.h>

#if defined(PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc (hwnd, msg, wparam, lparam);
            break;
        }
    }
    return 0;
}

#elif defined(PLATFORM_BLACKBERRY)
#include <bps/screen.h>
#elif defined(PLATFORM_ANDROID)
#include <android/native_activity.h>
EGLNativeWindowType native_window_external = nullptr;
std::mutex native_window_external_mutex;
void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window)
{
    std::lock_guard<std::mutex> lock(native_window_external_mutex);
    native_window_external = window;
}
void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window)
{
    std::lock_guard<std::mutex> lock(native_window_external_mutex);
    native_window_external = nullptr;
}
#elif defined(PLATFORM_RASBERRYPI)
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
#endif

namespace zeug
{
    window::window()
    {
        this->native_display_internal = EGL_DEFAULT_DISPLAY;
#if defined(PLATFORM_WINDOWS)
        WNDCLASS windowclass{0};
        HINSTANCE instance = GetModuleHandle(nullptr);

        if (!GetClassInfo(instance, "", &windowclass))
        {
            windowclass.lpszClassName    = "";
            windowclass.lpfnWndProc      = wndproc;
            windowclass.hInstance        = instance;
            windowclass.hbrBackground    = (HBRUSH)COLOR_BACKGROUND;

            if (!RegisterClass(&windowclass))
            {
                std::runtime_error e(zeug::util::win_errstr());
                throw e;
            }
        }

       RECT desktop;
       GetWindowRect(GetDesktopWindow(), &desktop);
       this->width_internal = desktop.right;
       this->height_internal = desktop.bottom;

        this->native_window_internal = CreateWindow(STRINGIFY(APP_NAME), STRINGIFY(APP_NAME), WS_OVERLAPPEDWINDOW|WS_VISIBLE, 0, 0, this->width_internal, this->height_internal, 0, 0, instance, nullptr);
	    if (!this->native_window_internal) 
		{
            std::runtime_error e(zeug::util::win_errstr());
            throw e;
        }	
        this->native_display_internal = GetDC(this->native_window_internal);
        if (!this->native_display_internal) 
        {
            std::runtime_error e(zeug::util::win_errstr());
            throw e;
        }   

        // Fullscreen
        WINDOWPLACEMENT window_place = { sizeof(window_place) };
        MONITORINFO monitor_info = { sizeof(monitor_info) };
        GetWindowPlacement(this->native_window_internal, &window_place);
        GetMonitorInfo(MonitorFromWindow(this->native_window_internal,MONITOR_DEFAULTTOPRIMARY), &monitor_info);
        DWORD style = GetWindowLong(this->native_window_internal, GWL_STYLE);
        SetWindowLong(this->native_window_internal, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(this->native_window_internal, HWND_TOP,
            monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
            monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
            monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

#elif defined(PLATFORM_BLACKBERRY)
        screen_context_t screen_ctx;
        screen_create_context(&screen_ctx,0);
        screen_window_t screen_win;
        screen_create_window(&screen_win, screen_ctx);
        
        this->native_window_internal = static_cast<EGLNativeWindowType>(screen_win);
        this->native_context_internal = static_cast<void*>(screen_ctx);
#elif defined(PLATFORM_ANDROID)
        while(!this->native_window_internal)
        {
            std::lock_guard<std::mutex> lock(native_window_external_mutex);
            this->native_window_internal = native_window_external;
        }
#elif defined(PLATFORM_RASBERRYPI)
        zeug::dynapi::dispman::init();
        using namespace zeug::dynapi::dispman::api;

        DISPMANX_ELEMENT_HANDLE_T dispman_element;
        DISPMANX_DISPLAY_HANDLE_T dispman_display;
        DISPMANX_UPDATE_HANDLE_T dispman_update;
        DISPMANX_MODEINFO_T dispman_modeinfo;
        EGL_DISPMANX_WINDOW_T dispman_window;

        VC_RECT_T dst_rect;
        VC_RECT_T src_rect;

        dst_rect.x = 0;
        dst_rect.y = 0;
        src_rect.x = 0;
        src_rect.y = 0;

        dispman_display = vc_dispmanx_display_open( 0 );
        dispman_update = vc_dispmanx_update_start( 0 );

        vc_dispmanx_display_get_info( dispman_display, &dispman_modeinfo);

        this->width_internal = dispman_modeinfo.width;
        this->height_internal = dispman_modeinfo.height;
        dst_rect.width = this->width_internal;
        dst_rect.height = this->height_internal;
        src_rect.width = this->width_internal << 16;
        src_rect.height = this->height_internal << 16;

        dispman_element = vc_dispmanx_element_add( dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, NULL, NULL, DISPMANX_NO_ROTATE);

        dispman_window.element = dispman_element;
        dispman_window.width = this->width_internal;
        dispman_window.height = this->height_internal;
        vc_dispmanx_update_submit_sync( dispman_update );

        this->native_window_internal = &dispman_window;

        zeug::dynapi::dispman::kill();
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
        zeug::dynapi::x11::init();
        using namespace zeug::dynapi::x11::api;
         
        this->native_display_internal = XOpenDisplay(nullptr);
        if (!this->native_display_internal)
        {
            std::runtime_error e("X11: Could not open display.");
            throw e;
        }

        this->width_internal = XWidthOfScreen(XDefaultScreenOfDisplay(this->native_display_internal));
        this->height_internal = XHeightOfScreen(XDefaultScreenOfDisplay(this->native_display_internal));

        this->native_window_internal = XCreateSimpleWindow(this->native_display_internal, XRootWindow(this->native_display_internal, XDefaultScreen(this->native_display_internal)), 0, 0, this->width_internal, this->height_internal, 0, XBlackPixel(this->native_display_internal, XDefaultScreen(this->native_display_internal)), XWhitePixel(this->native_display_internal, XDefaultScreen(this->native_display_internal)));
        if (!this->native_window_internal)
        {
            std::runtime_error e("X11: Could not create window.");
            throw e;
        }
        XMapWindow(this->native_display_internal, this->native_window_internal);
        XStoreName(this->native_display_internal, this->native_window_internal, const_cast<char*>(APP_NAME_STRING));

        XAtom wm_del_msg = XInternAtom(this->native_display_internal, const_cast<char*>("WM_DELETE_WINDOW"), false);
        XSetWMProtocols(this->native_display_internal, this->native_window_internal, &wm_del_msg, 1);

        // Disable decorations
        XAtom mwm_prop_hints = XInternAtom(this->native_display_internal, const_cast<char*>("_MOTIF_WM_HINTS"), true);
        long mwm_hints[5] = { 2, 0, 0, 0, 0 };
        XChangeProperty(this->native_display_internal, this->native_window_internal, mwm_prop_hints, mwm_prop_hints, 32, 0, reinterpret_cast<unsigned char *>(&mwm_hints), 5);

        // Fullscreen
        XAtom netwm_prop_hints = XInternAtom(this->native_display_internal, const_cast<char*>("_NET_WM_STATE"), false);
        XAtom netwm_hints[3];
        netwm_hints[0] = XInternAtom(this->native_display_internal, const_cast<char*>("_NET_WM_STATE_FULLSCREEN"), false);
        netwm_hints[1] = XInternAtom(this->native_display_internal, const_cast<char*>("_NET_WM_STATE_MAXIMIZED_HORZ"), false);
        netwm_hints[2] = XInternAtom(this->native_display_internal, const_cast<char*>("_NET_WM_STATE_MAXIMIZED_HORZ"), false);
        XChangeProperty(this->native_display_internal, this->native_window_internal, netwm_prop_hints, 4, 32, 0, reinterpret_cast<unsigned char *>(&netwm_hints), 3);
        XFlush(this->native_display_internal);
#endif
    }

    window::~window()
    {
#if defined(PLATFORM_WINDOWS)
        ReleaseDC(this->native_window_internal, this->native_display_internal);
        DestroyWindow(this->native_window_internal);
#elif defined(PLATFORM_BLACKBERRY)
        screen_destroy_window(static_cast<screen_window_t>(this->native_window_internal);
        screen_destroy_context(static_cast<screen_context_t>(this->native_context_internal);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_EMSCRIPTEN) || defined(PLATFORM_RASBERRYPI)
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
        using namespace zeug::dynapi::x11::api;
        XCloseDisplay(this->native_display_internal);
        zeug::dynapi::x11::kill();
#endif
    } 

    EGLNativeWindowType window::native_window(const EGLint format) 
    {
#if defined(PLATFORM_ANDROID)
        ANativeWindow_setBuffersGeometry(this->native_window_internal, this->width_internal, this->height_internal, format);
#endif
        return this->native_window_internal;
    }

    EGLNativeDisplayType window::native_display()
    {
        return this->native_display_internal;
    }

    std::uint32_t  window::width() 
    { 
#if defined(PLATFORM_ANDROID)
         this->width_internal = ANativeWindow_getWidth(this->native_window_internal);
#endif
        return this->width_internal;
    }

    std::uint32_t window::height() 
    { 
#if defined(PLATFORM_ANDROID)
         this->height_internal = ANativeWindow_getHeight(this->native_window_internal);
#endif
        return this->height_internal;
    }
    
    bool window::poll()
    {
#if defined(PLATFORM_WINDOWS)
        MSG msg;
        GetMessage( &msg, NULL, 0, 0 );
        switch ( msg.message )
        {    
            case WM_CLOSE:
            case WM_DESTROY:
            case WM_QUIT:
            {
                ::ShowWindow(this->native_window_internal, SW_HIDE);
                return false;
                break;
            }
            default: 
            {
                DispatchMessage(&msg);
                break;
            }
        } 
#elif defined(PLATFORM_ANDROID)
        std::lock_guard<std::mutex> lock(native_window_external_mutex);
        this->native_window_internal = native_window_external;
        if(!this->native_window_internal)
        {
            return false;
        }
        return true;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)
        if(this->native_display_internal)
        {
            using namespace zeug::dynapi::x11::api;
            while(XPending(this->native_display_internal) > 0)
            {
                XEvent event;
                XNextEvent(this->native_display_internal,&event);
                switch (event.type)
                {
                    case ConfigureNotify:
                    {
                        XConfigureEvent* cfg= reinterpret_cast<XConfigureEvent*>(&event);
                        this->width_internal = cfg->width;
                        this->height_internal = cfg->height; 
                        break;
                    }
                    case ClientMessage:
                    {
                        if(event.xclient.data.l[0] == static_cast<long>(XInternAtom(this->native_display_internal, const_cast<char*>("WM_DELETE_WINDOW"), false)))
                        {
                            XDestroyWindow(this->native_display_internal, this->native_window_internal);
                            return false;
                            break;
                        }
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }
#endif
        return true;
    }
}

