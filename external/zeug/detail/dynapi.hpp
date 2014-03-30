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
        // CUDA
        namespace cuda
        { 
            namespace api
            {
                enum cudaError
                {
                    cudaSuccess                           =      0,   ///< No errors
                    cudaErrorMissingConfiguration         =      1,   ///< Missing configuration error
                    cudaErrorMemoryAllocation             =      2,   ///< Memory allocation error
                    cudaErrorInitializationError          =      3,   ///< Initialization error
                    cudaErrorLaunchFailure                =      4,   ///< Launch failure
                    cudaErrorPriorLaunchFailure           =      5,   ///< Prior launch failure
                    cudaErrorLaunchTimeout                =      6,   ///< Launch timeout error
                    cudaErrorLaunchOutOfResources         =      7,   ///< Launch out of resources error
                    cudaErrorInvalidDeviceFunction        =      8,   ///< Invalid device function
                    cudaErrorInvalidConfiguration         =      9,   ///< Invalid configuration
                    cudaErrorInvalidDevice                =     10,   ///< Invalid device
                    cudaErrorInvalidValue                 =     11,   ///< Invalid value
                    cudaErrorInvalidPitchValue            =     12,   ///< Invalid pitch value
                    cudaErrorInvalidSymbol                =     13,   ///< Invalid symbol
                    cudaErrorMapBufferObjectFailed        =     14,   ///< Map buffer object failed
                    cudaErrorUnmapBufferObjectFailed      =     15,   ///< Unmap buffer object failed
                    cudaErrorInvalidHostPointer           =     16,   ///< Invalid host pointer
                    cudaErrorInvalidDevicePointer         =     17,   ///< Invalid device pointer
                    cudaErrorInvalidTexture               =     18,   ///< Invalid texture
                    cudaErrorInvalidTextureBinding        =     19,   ///< Invalid texture binding
                    cudaErrorInvalidChannelDescriptor     =     20,   ///< Invalid channel descriptor
                    cudaErrorInvalidMemcpyDirection       =     21,   ///< Invalid memcpy direction
                    cudaErrorAddressOfConstant            =     22,   ///< Address of constant error
                    cudaErrorTextureFetchFailed           =     23,   ///< Texture fetch failed
                    cudaErrorTextureNotBound              =     24,   ///< Texture not bound error
                    cudaErrorSynchronizationError         =     25,   ///< Synchronization error
                    cudaErrorInvalidFilterSetting         =     26,   ///< Invalid filter setting
                    cudaErrorInvalidNormSetting           =     27,   ///< Invalid norm setting
                    cudaErrorMixedDeviceExecution         =     28,   ///< Mixed device execution
                    cudaErrorCudartUnloading              =     29,   ///< CUDA runtime unloading
                    cudaErrorUnknown                      =     30,   ///< Unknown error condition
                    cudaErrorNotYetImplemented            =     31,   ///< Function not yet implemented
                    cudaErrorMemoryValueTooLarge          =     32,   ///< Memory value too large
                    cudaErrorInvalidResourceHandle        =     33,   ///< Invalid resource handle
                    cudaErrorNotReady                     =     34,   ///< Not ready error
                    cudaErrorInsufficientDriver           =     35,   ///< CUDA runtime is newer than driver
                    cudaErrorSetOnActiveProcess           =     36,   ///< Set on active process error
                    cudaErrorNoDevice                     =     38,   ///< No available CUDA device
                    cudaErrorStartupFailure               =   0x7f,   ///< Startup failure
                    cudaErrorApiFailureBase               =  10000    ///< API failure base
                };
                typedef enum cudaError cudaError_t;
                struct cudaDeviceProp 
                {
                    char name[256];
                    size_t totalGlobalMem;
                    size_t sharedMemPerBlock;
                    int regsPerBlock;
                    int warpSize;
                    size_t memPitch;
                    int maxThreadsPerBlock;
                    int maxThreadsDim[3];
                    int maxGridSize[3];
                    int clockRate;
                    size_t totalConstMem;
                    int major;
                    int minor;
                    size_t textureAlignment;
                    size_t texturePitchAlignment;
                    int deviceOverlap;
                    int multiProcessorCount;
                    int kernelExecTimeoutEnabled;
                    int integrated;
                    int canMapHostMemory;
                    int computeMode;
                    int maxTexture1D;
                    int maxTexture1DLinear;
                    int maxTexture2D[2];
                    int maxTexture2DLinear[3];
                    int maxTexture2DGather[2];
                    int maxTexture3D[3];
                    int maxTextureCubemap;
                    int maxTexture1DLayered[2];
                    int maxTexture2DLayered[3];
                    int maxTextureCubemapLayered[2];
                    int maxSurface1D;
                    int maxSurface2D[2];
                    int maxSurface3D[3];
                    int maxSurface1DLayered[2];
                    int maxSurface2DLayered[3];
                    int maxSurfaceCubemap;
                    int maxSurfaceCubemapLayered[2];
                    size_t surfaceAlignment;
                    int concurrentKernels;
                    int ECCEnabled;
                    int pciBusID;
                    int pciDeviceID;
                    int pciDomainID;
                    int tccDriver;
                    int asyncEngineCount;
                    int unifiedAddressing;
                    int memoryClockRate;
                    int memoryBusWidth;
                    int l2CacheSize;
                    int maxThreadsPerMultiProcessor;
                };
                static std::function<cudaError_t (struct cudaDeviceProp*, int)> cudaGetDeviceProperties;
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
                    detail::lib = std::make_unique<zeug::shared_lib>("cudart");

                    api::cudaGetDeviceProperties = detail::lib->function<api::cudaError_t (struct api::cudaDeviceProp*, int)>("cudaGetDeviceProperties");
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

        // EGL
        namespace egl
        { 
            namespace api
            {
                static std::function<EGLDisplay (EGLNativeDisplayType)> eglGetDisplay;
                static std::function<EGLBoolean (EGLDisplay, EGLint*, EGLint*)> eglInitialize;
                static std::function<EGLBoolean (EGLDisplay, const EGLint*, EGLConfig*, EGLint, EGLint*)> eglChooseConfig;
                static std::function<EGLContext (EGLDisplay, EGLConfig, EGLContext, const EGLint*)> eglCreateContext;
                static std::function<EGLSurface (EGLDisplay, EGLConfig, const EGLint*)> eglCreatePbufferSurface;
                static std::function<EGLBoolean (EGLDisplay, EGLSurface, EGLSurface, EGLContext)> eglMakeCurrent;
                static std::function<EGLBoolean (EGLDisplay, EGLContext)> eglDestroyContex;
                static std::function<EGLBoolean (EGLDisplay)> eglTerminate;
                static std::function<void* (const char*)> eglGetProcAddress;
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
                    detail::lib = std::make_unique<zeug::shared_lib>("EGL");

                    api::eglGetDisplay = detail::lib->function<EGLDisplay (NativeDisplayType)>("eglGetDisplay");
                    api::eglInitialize = detail::lib->function<EGLBoolean (EGLDisplay, EGLint*, EGLint*)>("eglInitialize");
                    api::eglChooseConfig = detail::lib->function<EGLBoolean (EGLDisplay, const EGLint*, EGLConfig*, EGLint, EGLint*)>("eglChooseConfig");
                    api::eglCreateContext = detail::lib->function<EGLContext (EGLDisplay, EGLConfig, EGLContext, const EGLint*)>("eglCreateContext");
                    api::eglCreatePbufferSurface = detail::lib->function<EGLSurface (EGLDisplay, EGLConfig, const EGLint*)>("eglCreatePbufferSurface");
                    api::eglMakeCurrent = detail::lib->function<EGLBoolean (EGLDisplay, EGLSurface, EGLSurface, EGLContext)>("eglMakeCurrent");
                    api::eglDestroyContex = detail::lib->function<EGLBoolean (EGLDisplay, EGLContext)>("eglDestroyContext");
                    api::eglTerminate = detail::lib->function<EGLBoolean (EGLDisplay)>("eglTerminate");
                    api::eglGetProcAddress = detail::lib->function<void* (const char*)>("eglGetProcAddress");
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

        // SDL2
        namespace sdl2
        { 
            namespace api
            {
                typedef enum
                {
                    SDL_POWERSTATE_UNKNOWN,
                    SDL_POWERSTATE_ON_BATTERY,
                    SDL_POWERSTATE_NO_BATTERY,
                    SDL_POWERSTATE_CHARGING,
                    SDL_POWERSTATE_CHARGED
                } SDL_PowerState;

                static std::function<SDL_PowerState (std::int32_t*, std::int32_t*)> SDL_GetPowerInfo;
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
                    detail::lib = std::make_unique<zeug::shared_lib>("SDL2");
                    api::SDL_GetPowerInfo = detail::lib->function<api::SDL_PowerState (std::int32_t*, std::int32_t*)>("SDL_GetPowerInfo");
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

        // ADL
        namespace adl
        { 
            namespace api
            {
                typedef struct version_info_t
                {
                    char version_verbose[256];
                    char version[256];
                    char weblink[256];

                } adl_version_info_t;

                static std::function<int* (adl_version_info_t*)> ADL_Graphics_Versions_Get;
            }
            namespace detail
            {
                static std::unique_ptr<zeug::shared_lib> lib;
                static std::uint32_t refcnt{};
                static std::mutex mutex{};

                inline void* COMPILER_STDCALL adl_malloc ( int size )
                {
                    return malloc ( size );
                }
            }

            static inline void init()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                if(!detail::lib)
                {
                    typedef void* ( COMPILER_STDCALL * malloc_callback_t)( int );
                    try
                    {
                        detail::lib = std::make_unique<zeug::shared_lib>("atiadlxx");
                    }
                    catch(...)
                    {
                        detail::lib = std::make_unique<zeug::shared_lib>("atiadlxy");
                    }
                    auto ADL_Main_Control_Create = detail::lib->function<int* (malloc_callback_t, int)>("ADL_Main_Control_Create");
                    ADL_Main_Control_Create (detail::adl_malloc, 1);

                    api::ADL_Graphics_Versions_Get = detail::lib->function<int* (api::adl_version_info_t*)>("ADL_Graphics_Versions_Get");
                }
                detail::refcnt++;
            }
            static inline void kill()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                detail::refcnt--;
                if(detail::refcnt == 0)
                {
                    auto ADL_Main_Control_Destroy = detail::lib->function<int* ()>("ADL_Main_Control_Destroy");
                    ADL_Main_Control_Destroy ();
                    detail::lib.release();
                }
            }
        }

        // NVAPI
        namespace nvapi
        { 
            namespace api
            {
                typedef char nvapi_short_str_t[64];
                static std::function<int* (unsigned int*, nvapi_short_str_t)> NvAPI_SYS_GetDriverAndBranchVersion;
            }
            namespace detail
            {
                static std::unique_ptr<zeug::shared_lib> lib;
                static std::uint32_t refcnt{};
                static std::mutex mutex{};

                template<class T>
                inline std::function<T> nvapi_queryinterface(unsigned int id)
                {
                    auto NvAPI_QueryInterface = lib->function<int* (unsigned int)>("NvAPI_QueryInterface");
                    void* raw_ptr = NvAPI_QueryInterface(id);
                    T* func_ptr = nullptr;
                    std::memcpy(&func_ptr, &raw_ptr, sizeof(raw_ptr));
                    return func_ptr;
                }
            }

            static inline void init()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                if(!detail::lib)
                {
                    detail::lib = std::make_unique<zeug::shared_lib>("nvapi");
                    auto NvAPI_Initialize = detail::nvapi_queryinterface<int* ()>(0x0150E828);
                    NvAPI_Initialize();

                    api::NvAPI_SYS_GetDriverAndBranchVersion = detail::nvapi_queryinterface<int* (unsigned int*, api::nvapi_short_str_t)>(0x2926AAAD);
                }
                detail::refcnt++;
            }
            static inline void kill()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                detail::refcnt--;
                if(detail::refcnt == 0)
                {
                    auto NvAPI_Unload = detail::nvapi_queryinterface<int* ()>(0xD22BDD7E);
                    NvAPI_Unload();
                    detail::lib.release();
                }
            }
        }

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

        // XNVCtrl
        namespace xnvctrl
        { 
            namespace api
            {
                static std::function<bool (XDisplay*, int*, int*)> XNVCTRLQueryExtension;
                static std::function<bool (XDisplay*, int)> XNVCTRLIsNvScreen;
                static std::function<bool (XDisplay*, int, unsigned int, unsigned int, char**)> XNVCTRLQueryStringAttribute;
            }
            namespace detail
            {
                static std::unique_ptr<zeug::shared_lib> lib;
                static std::uint32_t refcnt{};
                static std::mutex mutex{};
            }

            static void init()
            {
                std::lock_guard<std::mutex> lock(detail::mutex);
                if(!detail::lib)
                {
                    detail::lib = std::make_unique<zeug::shared_lib>("XNVCtrl");
                    api::XNVCTRLQueryExtension = detail::lib->function<bool (XDisplay*, int*, int*)>("XNVCTRLQueryExtension");
                    api::XNVCTRLIsNvScreen = detail::lib->function<bool (XDisplay*, int)>("XNVCTRLIsNvScreen");
                    api::XNVCTRLQueryStringAttribute = detail::lib->function<bool (XDisplay*, int, unsigned int, unsigned int, char**)>("XNVCTRLQueryStringAttribute");
                }
                detail::refcnt++;
            }
            static void kill()
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