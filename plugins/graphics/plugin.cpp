// Public Domain
#include "plugins/graphics/plugin.hpp"
#include "base/platform.hpp"
#include "base/hash/stringhash.hpp"
#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"
#include "plugins/graphics/error.hpp"
#include "plugins/graphics/render.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GLCOREARB_PROTOTYPES
#include <GL/glcorearb.h>
#include <oglplus/error.hpp>
#include <zmq.hpp>

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer(new GraphicsPlugin(base_window, zmq_context));
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Graphics = { &InitPlugin};
}

GraphicsPlugin::GraphicsPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{        
}

GraphicsPlugin::~GraphicsPlugin()
{      
}

/* Plugin: Multithreaded loop. */
void GraphicsPlugin::Loop()
{
    try
    {
        /* ZMQ: Wait a bit for other plugins to etablish sockets. */
        std::chrono::milliseconds duration( 100 );
        std::this_thread::sleep_for( duration );
        
        /* ZMQ: Create input subscription socket on this thread. */
        zmq::socket_t zmq_input_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_input_subscriber.connect("inproc://input");

        /* ZMQ: Suscribe to all messages. */
        zmq_input_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        /* EGL: Set configuration variables. */
        EGLint egl_attributes[] =
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_NONE
        };
        
        EGLint egl_context_attributes[] =
        { 
            //EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
            //EGL_CONTEXT_MINOR_VERSION_KHR, 2,
            //EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
            //EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
            EGL_NONE
        };

        /* EGL: Initialization. */
        auto egl_display = EGL_CheckError(eglGetDisplay( this->base_window.get()->GetNativeDisplay() ));  
        
        EGLint egl_major = 0; 
        EGLint egl_minor = 0;
        EGL_CheckError(eglInitialize(egl_display, &egl_major, &egl_minor));
        EGL_CheckError(eglBindAPI(EGL_OPENGL_API));
        
        std::cout << "EGL vendor: " << EGL_CheckError(eglQueryString(egl_display, EGL_VENDOR)) << std::endl;
        std::cout << "EGL version: " << EGL_CheckError(eglQueryString(egl_display, EGL_VERSION)) << std::endl;
        std::cout << "EGL client apis: " << EGL_CheckError(eglQueryString(egl_display, EGL_CLIENT_APIS)) << std::endl;   
        std::cout << "EGL extensions: " << EGL_CheckError(eglQueryString(egl_display, EGL_EXTENSIONS)) << std::endl;
        
        /* EGL: Configuraion. */
        EGLint egl_num_configs = 0;
        EGLConfig egl_config;
        EGL_CheckError(eglChooseConfig(egl_display, egl_attributes, &egl_config, 1, &egl_num_configs));

        /* EGL: Link to base::Window. */
        EGLint egl_format;
        eglGetConfigAttrib(egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
        auto egl_surface = EGL_CheckError(eglCreateWindowSurface(egl_display, egl_config, this->base_window.get()->GetNativeWindow(egl_format), NULL));  

        /* EGL: Context creation. */
        auto egl_context = EGL_CheckError(eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attributes));
        
        /* EGL: Make context current on this thread. */
        EGL_CheckError(eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context));
        
        /* OGL: Print version information */
        GLint gl_major = 0; 
        GLint gl_minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
        glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
        std::cout << "GL version: " << gl_major << "." << gl_minor << std::endl;
        
        /* OGL: Initialization. */
        Render ogl_render;
        
        /* Plugin: Loop. */
        std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point newtime = std::chrono::high_resolution_clock::now();
        float akkumulator = 0.0f;
        
        for(;;)
        {
            /* ZMQ: Listen. */
            zmq::message_t zmq_message;
            if (zmq_input_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (base::StringHash("STOP") == base::StringHash(zmq_message.data()))
                {
                    std::cout<< "ZMQ: GraphicsPlugin received STOP signal." << std::endl;
                    break;
                }
            }
            
            /* Plugin: Force 120hz rendering*/
            newtime = std::chrono::high_resolution_clock::now();
            auto deltatime = std::chrono::duration<float, std::ratio<1>>(newtime - oldtime).count();
            oldtime = newtime;  
            akkumulator += deltatime;
            
            if( akkumulator > (1.0f / 120.0f))
            {
                deltatime = akkumulator;
                akkumulator = 0.0f;
            
                /* OGL: Draw. */
                ogl_render.Draw( /*deltatime*/);
            
                /* EGL: Swap buffers. */
                EGL_CheckError(eglSwapBuffers ( egl_display, egl_surface ));
            }
            else
            {
                std::this_thread::yield();
            }
        }
        /* EGL: Cleanup. */
        EGL_CheckError(eglDestroyContext(egl_display, egl_context ));
        EGL_CheckError(eglDestroySurface(egl_display, egl_surface ));
        EGL_CheckError(eglTerminate(egl_display));
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(const oglplus::Error& err)
    {
        std::stringstream error_string; 
        error_string << "Error (in " << 
            err.GLSymbol() << ", " <<
            err.ClassName() << ": '" <<
            err.ObjectDescription() << "'): " <<
            err.what() << " [" << 
            err.File() << ":" << 
            err.Line() << "] " << 
            std::endl;
        err.Cleanup();   
        throw std::runtime_error(error_string.str());
    }
}
