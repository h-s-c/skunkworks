// Public Domain
#include <zeug/platform.hpp>
#include <zeug/detail/platform_macros.hpp>
#include <zeug/detail/dynapi.hpp>
#include <zeug/detail/stdfix.hpp>
#include <zeug/shared_lib.hpp>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <EGL/egl.h>

#if defined(PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#elif defined(PLATFORM_BLACKBERRY)
#elif defined(PLATFORM_ANDROID)
#include <zeug/thirdparty/cpu-features.h>
#elif defined(PLATFORM_EMSCRIPTEN)
#elif defined(PLATFORM_RASBERRYPI)
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
#include <cpuid.h>
#endif

namespace zeug
{
  namespace platform
  {
    std::string arch()
    {
#if defined(PLATFORM_WINDOWS)
        BOOL bIsWow64 = FALSE;
        typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
        LPFN_ISWOW64PROCESS fnIsWow64Process;
        fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
        if(NULL != fnIsWow64Process)
        {
            fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
            if(bIsWow64) return "x86-64";
        }
#endif

#if defined(PLATFORM_ARCH_X86_64)
        return "x86-64";
#elif defined(PLATFORM_ARCH_X86)
        return "x86";
#elif defined(PLATFORM_ARCH_ARM_64)
        return "AArch64";
#elif defined(PLATFORM_ARCH_ARM)
        return "AArch32";
#else
        return "Unknown";
#endif
    }

    std::string name()
    {
#if defined(PLATFORM_WINDOWS)
        return "Windows";
#elif defined(PLATFORM_BLACKBERRY)
        return "BlackBerry";
#elif defined(PLATFORM_ANDROID)
        return "Android";
#elif defined(PLATFORM_EMSCRIPTEN)
        return "asm.js";
#elif defined(PLATFORM_RASBERRYPI)
        return "Raberry Pi";
#elif defined(PLATFORM_BSD)
        return "BSD";
#elif defined(PLATFORM_LINUX)
        return "Linux";
#else
        return "Unknown";
#endif
    }

    std::string verbose()
    {
        auto platform = std::string("-----Platform-----\n") + 
            "Arch: " + arch() + "\n" +
            "Name: " + name() + "\n";

        auto app = std::string("-----Application-----\n") + 
            "Arch: " + this_app::arch() + "\n" + 
            "Compiler: " + this_app::compiler() + "\n" + 
            "Compiler version: " + std::to_string(this_app::compiler_version().major) + 
            "." + std::to_string(this_app::compiler_version().minor) + 
            "." + std::to_string(this_app::compiler_version().patch) + "\n" + 
            "Standard Library: " + this_app::stdlib() + "\n";

        auto thread = std::to_string(std::thread::hardware_concurrency());
        if(this_cpu::htt()) thread += " (HT)";

        auto battery = std::string("-----Battery-----\n") + "Status: ";
        switch(this_battery::state())
        {
            case zeug::powerstate::unknown : battery += "Unknown\n"; break;
            case zeug::powerstate::on_battery : battery += "On Battery\n"; break;
            case zeug::powerstate::no_battery : battery += "No Battery\n"; break;
            case zeug::powerstate::charging : battery += "Charging\n"; break;
            case zeug::powerstate::charged : battery += "Charged\n"; break;
            default : battery += "Unknown(Error)\n"; break;
        }

        battery += "Remaining: ";
        if(!this_battery::percent().first) battery += "Unknown";
        else battery += std::to_string(this_battery::percent().second) + "%";

        if (!this_battery::seconds().first) battery += "\n";
        else battery += " (" + std::to_string(this_battery::seconds().second.count()) + "s)\n";

        auto cpu = std::string("-----CPU-----\n") + 
            "Arch: " + this_cpu::arch() + "\n" + 
            "Threads: " + thread + "\n" + 
            "Vendor: " + this_cpu::vendor() + "\n" + 
            "Model: " + this_cpu::model() + "\n";

        cpu += "L1 Cache: ";
        if(!this_cpu::l1_size().first) cpu += "Unknown\n";
        else  cpu += std::to_string(this_cpu::l1_size().second) + "KB\n";
                            
        cpu += "Byteorder: ";
        if(this_cpu::byteorder_little()) cpu += "Little Endian";
        if(this_cpu::byteorder_big()) cpu += "Big Endian";
        cpu += "\n";

        cpu += "Extensions: ";
        if(this_cpu::sse1()) cpu += "SSE ";
        if(this_cpu::sse2()) cpu += "SSE2 ";
        if(this_cpu::sse3()) cpu += "SSE3 ";
        if(this_cpu::ssse3()) cpu += "SSSE3 ";
        if(this_cpu::sse41()) cpu += "SSE4.1 ";
        if(this_cpu::sse42()) cpu += "SSE4.2 ";
        if(this_cpu::sse4a()) cpu += "SSE4a ";
        if(this_cpu::aes()) cpu += "AES ";
        if(this_cpu::avx()) cpu += "AVX ";
        if(this_cpu::avx2()) cpu += "AVX2 ";
        if(this_cpu::xop()) cpu += "XOP ";
        if(this_cpu::fma3()) cpu += "FMA3 ";
        if(this_cpu::fma4()) cpu += "FMA4 ";
        if(this_cpu::neon()) cpu += "NEON ";
        cpu += "\n";

        auto gpu = std::string("-----GPU-----\n") + 
            "Vendor: " + this_gpu::vendor() + "\n" +
            "Model: " + this_gpu::model() + "\n" +
            "Driver vendor: " + this_gpu::driver_vendor() + "\n";

        gpu += "Driver version: ";
        if(this_gpu::driver_version().major == 0) gpu += "Unknown\n";
        else  gpu += std::to_string(this_gpu::driver_version().major) + "." + std::to_string(this_gpu::driver_version().minor);

        return platform + app + battery + cpu + gpu;
    }
  }

  namespace this_app
  {
    std::string arch()
    {
#if defined(PLATFORM_ARCH_X86_64)
        return "x86-64";
#elif defined(PLATFORM_ARCH_X86)
        return "x86";
#elif defined(PLATFORM_ARCH_ARM_64)
        return "AArch64";
#elif defined(PLATFORM_ARCH_ARM)
        return "AArch32";
#else
        return "Unknown";
#endif
    }

    std::string compiler()
    {
#if defined(COMPILER_CLANG)
        auto name = "Clang";
#elif defined(COMPILER_ICC)
        auto name = "Intel C++";
#elif defined(COMPILER_MSVC)
        auto name = "Visual C++";
#elif defined(COMPILER_GCC)
        auto name = "GCC";
#else
        auto name = "Unknown";
#endif
        return std::string(name);
    }

    zeug::version_t compiler_version()
    {
        return zeug::version_t{COMPILER_VERSION_MAJOR, COMPILER_VERSION_MINOR,COMPILER_VERSION_PATCH};
    }

    std::string stdlib()
    {
#if defined(COMPILER_STDLIB_DINKUMWARE)
        return "Dinkumware";
#elif defined(COMPILER_STDLIB_GNU)
        return "libstdc++";
#elif defined(COMPILER_STDLIB_LIBCPP)
        return "libc++";
#else
        return "Unknown";
#endif
    }
  }

  namespace this_battery
  {
    namespace detail
    {
        std::tuple<zeug::powerstate, std::chrono::seconds, std::int32_t> powerinfo()
        {
            std::int32_t seconds = 0;
            std::int32_t percent = 0;
            zeug::powerstate powerstate = zeug::powerstate::unknown;

            try
            {
                zeug::dynapi::sdl2::init();
                using namespace zeug::dynapi::sdl2::api;

                std::array<zeug::powerstate, 5> translation_array =
                {{
                    zeug::powerstate::unknown, 
                    zeug::powerstate::on_battery, 
                    zeug::powerstate::no_battery, 
                    zeug::powerstate::charging, 
                    zeug::powerstate::charged
                }};
                powerstate = translation_array[SDL_GetPowerInfo(&seconds, &percent)];

                zeug::dynapi::sdl2::kill();
            }
            catch(...){}

            return std::make_tuple(powerstate, std::chrono::seconds(seconds), percent);
        }
    }

    zeug::powerstate state()
    {
        return std::get<0>(detail::powerinfo());
    }

    std::pair<bool, std::chrono::seconds> seconds()
    {
        auto seconds = std::get<1>(detail::powerinfo());
        return std::make_pair( seconds.count() != -1 && seconds.count() != 0 && seconds.count() != 255, seconds);
    }
    std::pair<bool, std::uint8_t> percent()
    {
        auto percent = std::get<2>(detail::powerinfo());
        return std::make_pair( percent != -1 && percent != 0 && percent != 255, percent);
    }
  }

  namespace this_cpu
  {
    namespace detail
    {
        COMPILER_FORCEINLINE std::array<std::int32_t, 4> cpuid(std::int32_t op) 
        {
            std::array<std::int32_t, 4> out = {{0,0,0,0}};
#if (defined(PLATFORM_ARCH_X86_64) || defined(PLATFORM_ARCH_X86)) && !defined(PLATFORM_EMSCRIPTEN)
#   if defined(COMPILER_GCC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC))  
            __cpuid(op, out[0], out[1], out[2], out[3]);
#   elif defined(COMPILER_CLANG)
            asm volatile ("cpuid" : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3]) : "a"(op));
#   elif defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
            __cpuid(out.data(), op); 
#   else
            __asm 
            {
                mov eax, op
                xor ecx, ecx
                cpuid;
                mov esi, out.data()
                mov [esi], eax
                mov [esi+4], ebx
                mov [esi+8], ecx
                mov [esi+12], edx
            }
#   endif
#endif
            return out;
        }
    }

    std::string arch()
    {
#if defined(PLATFORM_ANDROID)
        auto family = android_getCpuFamily();
        if(family == ANDROID_CPU_FAMILY_X86) return "x86";
        if(family == ANDROID_CPU_FAMILY_ARM) return "ARM";
        if(family == ANDROID_CPU_FAMILY_MIPS) return "MIPS";
        return "Unknown";
#else
        if ((detail::cpuid(0x80000001)[3] & (1 << 29)) != 0) return "x86-64";
        return "x86";
#endif
    }  

    std::string vendor()
    {
        auto info = detail::cpuid(0); 
        int name[4];
        name[0] = info[1];
        name[1] = info[3];
        name[2] = info[2];
        name[3] = 0;
        if (std::string(reinterpret_cast<char*>(name)) == "GenuineIntel") return "Intel";
        if (std::string(reinterpret_cast<char*>(name)) == "AuthenticAMD") return "AMD";
        return "Unknown";
    }

    std::string model()
    {
#if defined(PLATFORM_ARCH_X86_64) || defined(PLATFORM_ARCH_X86)
        auto info = detail::cpuid(1);
        int family = ((info[0] >> 8) & 0x0F) + ((info[0] >> 20) & 0xFF);
        int model  = ((info[0] >> 4) & 0x0F) | ((info[0] >> 12) & 0xF0);
        if (vendor() == "Intel")
        {
            if (family == 0x06)
            {
                if (model == 0x0E) return "Core 1";         // Core 1
                if (model == 0x0F) return "Core 2";         // Core 2, 65 nm
                if (model == 0x16) return "Core 2";         // Core 2, 65 nm Celeron
                if (model == 0x17) return "Core 2";         // Core 2, 45 nm
                if (model == 0x1A) return "Nehalem";        // Core i7, Xeon 3500/5500, Nehalem
                if (model == 0x1E) return "Nehalem";        // Core i5/i7, Xeon 3400
                if (model == 0x1F) return "Nehalem";        // Core i5/i7
                if (model == 0x2E) return "Nehalem";        // Xeon 6500/7500
                if (model == 0x17) return "Penryn";         // Penryn, 45 nm
                if (model == 0x25) return "Westmere";       // Westmere, 32 nm
                if (model == 0x2C) return "Westmere";       // Westmere, 32 nm
                if (model == 0x2F) return "Westmere";       // Xeon E7, Westmere, 32 nm
                if (model == 0x2A) return "SandyBridge";    // Sandy Bridge Core, 32 nm
                if (model == 0x2D) return "SandyBridge";    // Sandy Bridge Xeon, 32 nm
                if (model == 0x3A) return "IvyBridge";      // Ivy Bridge Core, 22 nm
                if (model == 0x3E) return "IvyBridge";      // Ivy Bridge Xeon, 22 nm
                if (model == 0x3C) return "Haswell";        // Haswell, 22 nm
                if (model == 0x3F) return "Haswell";        // Haswell, 22 nm
                if (model == 0x45) return "Haswell";        // Haswell, 22 nm
                if (model == 0x46) return "Haswell";        // Haswell, 22 nm
                return "Unknown (New)";
            }
            if (family == 0x0B) return "Many Integrated Core";  // Xeon Phi
            if (family < 0x06) return "Unknown (Old)";          // earlier than P6
        }
        else if (vendor() == "AMD")
        {
            if (family <  20) return "Unknown (Old)";   // K10 and earlier
            if (family == 20) return "Bobcat";          // Bobcat, 40 nm
            if (family == 21) return "Bulldozer";       // Bulldozer, 32 nm
            if (family == 22) return "Jaguar";          // Jaguar, 28 nm
            return "Unknown (New)";
        }
#endif
        return "Unknown";
    }

    std::pair<bool, std::uint32_t> l1_size()
    {
        if (vendor() == "Intel")
        {
            return std::make_pair( true, ((detail::cpuid(1)[1] >> 8) & 0xff) * 8);
        }
        else if (vendor() == "AMD")
        {
            return std::make_pair( true, detail::cpuid(0x80000005)[2] & 0xff);
        }
        else return std::make_pair( false, 0);
    } 

    bool byteorder_little()
    {
        union 
        {
            std::uint8_t  c[4];
            std::uint32_t i;
        } u;
        u.i = 0x01020304;
        if (0x04 == u.c[0]) return true;
        return false;
    }

    bool byteorder_big()
    {
        union 
        {
            std::uint8_t  c[4];
            std::uint32_t i;
        } u;
        u.i = 0x01020304;
        if (0x01 == u.c[0]) return true;
        return false;
    }

    bool htt()
    {
        return (detail::cpuid(1)[3] & (1 << 28)) != 0;
    }

    bool sse1()
    {
        return (detail::cpuid(1)[3] & (1 << 25)) != 0;
    }

    bool sse2()
    {
        return (detail::cpuid(1)[3] & (1 << 26)) != 0;
    }

    bool sse3()
    {
        return (detail::cpuid(1)[2] & (1 << 0)) != 0;
    }
    
    bool ssse3()
    {
        return (detail::cpuid(1)[2] & (1 << 9)) != 0;
    }

    bool sse41()
    {
        return (detail::cpuid(1)[2] & (1 << 19)) != 0;
    }

    bool sse42()
    {
        return (detail::cpuid(1)[2] & (1 << 20)) != 0;
    }

    bool sse4a()
    {
        return (detail::cpuid(0x80000001)[2] & (1 << 6)) != 0;
    }

    bool aes()
    {
        return (detail::cpuid(1)[2] & (1 << 28)) != 0;
    }

    bool avx()
    {
        return (detail::cpuid(1)[2] & (1 << 28)) != 0;
    }

    bool avx2()
    {
        return (detail::cpuid(7)[1] & (1 << 5)) != 0;
    }

    bool xop()
    {
        return (detail::cpuid(0x80000001)[2] & (1 << 11)) != 0;
    }

    bool fma3()
    {
        return (detail::cpuid(1)[2] & (1 << 12)) != 0;
    }

    bool fma4()
    {
        return (detail::cpuid(0x80000001)[2] & (1 << 16)) != 0;
    }

    bool neon()   
    {
#if defined(PLATFORM_ANDROID)
        if(android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM)
        {
            if(android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) 
            {
                return true;
            }
        }
#endif
        return false;
    }
  }
  namespace this_gpu
  {
    namespace detail
    {
        static std::string vendor = "Unknown";
        static std::string model = "Unknown";
        static std::string driver_vendor = "Unknown";
        static zeug::version_t driver_version = {0U, 0U, 0U};

        static std::once_flag flag;

        zeug::version_t to_ver(const std::string& str)
        {  
            std::vector<std::uint32_t> vec;
            std::string substr = str;
            for(;;)
            {
                std::uint32_t ret = 0U;
                std::stringstream sstream;
                for(std::uint32_t j = 0; j < substr.size(); j++)
                {
                    if(substr.at(j) == '.') break;
                    sstream << substr.at(j);
                }
                sstream >> ret;
                vec.emplace_back(ret);
                if(substr.find(".") == std::string::npos) break;
                substr = substr.substr(substr.find(".")+1, std::string::npos);
            }
            for(;;)
            {
                if(vec.size() >= 3) break;
                vec.emplace_back(0U);
            } 
            return zeug::version_t{vec.at(0), vec.at(1), vec.at(2)};
        }

        void try_adl()
        {
            zeug::dynapi::adl::init();
            using namespace zeug::dynapi::adl::api;

            vendor = "AMD";
            driver_vendor = "AMD";

            adl_version_info_t version_info;
            ADL_Graphics_Versions_Get(&version_info);
            driver_version = to_ver(std::string(version_info.version));

            zeug::dynapi::adl::kill();
        }

        void try_cuda()
        {
            zeug::dynapi::cuda::init();        
            using namespace zeug::dynapi::cuda::api;

            vendor = "Nvidia";
            driver_vendor = "Nvidia";

            cudaDeviceProp devProp;
            cudaGetDeviceProperties(&devProp, 0);

            switch(devProp.major)
            {
                case 1:
                    model = "Tesla";
                    break;
                case 2:
                    model = "Fermi";
                    break;
                case 3:
                    model = "Kepler";
                    break;
                default:
                    model = "Unknown";
                    break;
            }

            zeug::dynapi::cuda::kill();
        }

        void try_ogl()
        {
            #if defined (PLATFORM_ANDROID)
            return;
            #endif
            
            zeug::dynapi::egl::init();
            using namespace zeug::dynapi::egl::api;
            EGLConfig eglConfigWindow = nullptr;
            auto eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(eglDisplay, nullptr, nullptr);
            eglChooseConfig(eglDisplay,  nullptr, &eglConfigWindow, 1, nullptr);
            auto eglContext = eglCreateContext(eglDisplay, eglConfigWindow, nullptr, nullptr);
            auto eglSurfacePbuffer = eglCreatePbufferSurface(eglDisplay,  nullptr, nullptr);

            typedef const std::uint8_t *(PFNGLGETSTRINGPROC) (std::uint32_t name);

            void* raw_ptr = eglGetProcAddress("glGetString");
            PFNGLGETSTRINGPROC* func_ptr = nullptr;
            std::memcpy(&func_ptr, &raw_ptr, sizeof(raw_ptr));
            
            auto glGetString = std::function<PFNGLGETSTRINGPROC>(func_ptr);

            std::string vendor_string = reinterpret_cast<const char*>(glGetString(0x1F00));
            std::string renderer_string = reinterpret_cast<const char*>(glGetString(0x1F01));
            std::string version_string = reinterpret_cast<const char*>(glGetString(0x1F02));

            std::transform(vendor_string.begin(), vendor_string.end(), vendor_string.begin(), tolower);
            std::transform(renderer_string.begin(), renderer_string.end(), renderer_string.begin(), tolower);
            std::transform(version_string.begin(), version_string.end(), version_string.begin(), tolower);

            auto pos = version_string.find("mesa");
            if(pos != std::string::npos)
            {
                driver_vendor = "Mesa";
                driver_version = to_ver(version_string.substr(pos+5, std::string::npos));
            }

            if((vendor_string.find("amd") != std::string::npos) || (renderer_string.find("amd") != std::string::npos))
            {
                vendor = "AMD";

                if( (renderer_string.find("cedar") != std::string::npos) ||
                    (renderer_string.find("redwood") != std::string::npos) ||
                    (renderer_string.find("juniper") != std::string::npos) ||
                    (renderer_string.find("cypress") != std::string::npos) ||
                    (renderer_string.find("sumo") != std::string::npos) ||
                    (renderer_string.find("sumo2") != std::string::npos))
                {
                    model = "Evergreen (VLIW5)";
                }
                if( (renderer_string.find("aruba") != std::string::npos) ||
                    (renderer_string.find("barts") != std::string::npos) ||
                    (renderer_string.find("turks") != std::string::npos) ||
                    (renderer_string.find("caicos") != std::string::npos))
                {
                    model = "Northern Islands (VLIW5)";
                }
                if( (renderer_string.find("cayman") != std::string::npos) ||
                    (renderer_string.find("antilles") != std::string::npos))
                {
                    model = "Northern Islands (VLIW4)";
                }
                if( (renderer_string.find("cape verde") != std::string::npos) ||
                    (renderer_string.find("pitcairn") != std::string::npos) ||
                    (renderer_string.find("tahiti") != std::string::npos) ||
                    (renderer_string.find("oland") != std::string::npos) ||
                    (renderer_string.find("hainan") != std::string::npos))
                {
                    model = "Southern Islands (GCN)";
                }
                if( (renderer_string.find("bonaire") != std::string::npos) ||
                    (renderer_string.find("kabini") != std::string::npos) ||
                    (renderer_string.find("kaveri") != std::string::npos))
                {
                    model = "Sea Islands (GCN)";
                }
                if( (renderer_string.find("hawaii") != std::string::npos))
                {
                    model = "Vulcanic Islands (GCN2)";
                }
            }
            if((vendor_string.find("nvidia") != std::string::npos) || (renderer_string.find("nvidia") != std::string::npos))
            {
                vendor = "Nvidia";

                if( (renderer_string.find("nv50") != std::string::npos) ||
                    (renderer_string.find("nv8") != std::string::npos) ||
                    (renderer_string.find("nv9") != std::string::npos) ||
                    (renderer_string.find("nva") != std::string::npos))
                {
                    model = "Tesla";
                }
                if( (renderer_string.find("nvc") != std::string::npos) ||
                    (renderer_string.find("nvd") != std::string::npos))
                {
                    model = "Fermi";
                }
                if( (renderer_string.find("nve") != std::string::npos) ||
                    (renderer_string.find("nvf") != std::string::npos) ||
                    (renderer_string.find("nv108") != std::string::npos))
                {
                    model = "Kepler";
                }
            }
            if((vendor_string.find("intel") != std::string::npos) || (renderer_string.find("intel") != std::string::npos))
            {
                vendor = "Intel";
            }
            if((vendor_string.find("imagination") != std::string::npos) || (renderer_string.find("imagination") != std::string::npos))
            {
                vendor = "Imagination Technologies";
            }
            if((vendor_string.find("qualcomm") != std::string::npos) || (renderer_string.find("qualcomm") != std::string::npos))
            {
                vendor = "Qualcomm";
            }
            if((vendor_string.find("arm") != std::string::npos) || (renderer_string.find("arm") != std::string::npos))
            {
                vendor = "ARM";
            }

            eglDestroyContex(eglDisplay, eglContext);
            eglTerminate(eglDisplay);

            zeug::dynapi::egl::kill();
        }

        void try_nvapi()
        {
            zeug::dynapi::nvapi::init(); 
            using namespace zeug::dynapi::nvapi::api;

            vendor = "Nvidia";   
            driver_vendor = "Nvidia";

            std::uint32_t version = 0;
            NvAPI_SYS_GetDriverAndBranchVersion(&version, nullptr);
            //driver_version = std::to_string(version);         

            zeug::dynapi::nvapi::kill();
        }

        void try_xnvctrl()
        {
#if defined(PLATFORM_BLACKBERRY)
#elif defined(PLATFORM_ANDROID)
#elif defined(PLATFORM_EMSCRIPTEN)
#elif defined(PLATFORM_RASBERRYPI)
#elif defined(PLATFORM_BSD) || defined(PLATFORM_LINUX)
            zeug::dynapi::x11::init();
            zeug::dynapi::xnvctrl::init();
            using namespace zeug::dynapi::x11::api;
            using namespace zeug::dynapi::xnvctrl::api;

            XDisplay* display = XOpenDisplay(nullptr);
            int event_base = 0, error_base = 0;
            if (XNVCTRLQueryExtension(display, &event_base, &error_base)) 
            {
                int screen_count = XScreenCount(display);
                for (int screen = 0; screen < screen_count; ++screen) 
                {
                    char* buffer = nullptr;
                    if (XNVCTRLIsNvScreen(display, screen) && XNVCTRLQueryStringAttribute(display, screen, 0, 3, &buffer)) 
                    {
                        vendor = "Nvidia";   
                        driver_vendor = "Nvidia";
                        //driver_version = std::string (buffer);
                        XFree(buffer);
                    }
                }
            }

            zeug::dynapi::xnvctrl::kill();
            zeug::dynapi::x11::kill();
#endif
        }

        void detect()
        {
            // Clang does not like this
            #if !defined(COMPILER_CLANG)
            std::call_once(flag, []()
            #endif
            {
                try{try_adl();} catch(...){};
                try{try_cuda();} catch(...){};
                try{try_ogl();} catch(...){};
                try{try_nvapi();} catch(...){};
                try{try_xnvctrl();} catch(...){};
            }
            #if !defined(COMPILER_CLANG)
            );
            #endif
        }
    }

    std::string vendor()
    {
        detail::detect();
        return detail::vendor;
    }

    std::string model()
    {
        detail::detect();
        return detail::model;
    }

    std::string driver_vendor()
    {
        detail::detect();
        return detail::driver_vendor;
    }

    zeug::version_t driver_version()
    {
        detail::detect();
        return detail::driver_version;
    }

    void driver_perf_profile(bool perf)
    {
        if(perf)
        {
        }
        else
        {
        }
    }
  }
}
