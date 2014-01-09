// Public Domain
#include "plugins/graphics/plugin.hpp"
#include "base/string/stringhash.hpp"
#include "framework/plugin_api.hpp"
#include "plugins/graphics/render.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <platt/platform.hpp>
#include <platt/window.hpp>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <zmq.hpp>

extern "C" { 
 COMPILER_DLLEXPORT std::uint32_t NvOptimusEnablement = 0x00000001;
}

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer = std::make_unique<GraphicsPlugin>(base_window, zmq_context);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Graphics = { &InitPlugin};
}

GraphicsPlugin::GraphicsPlugin(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{      
    /* ZMQ: Create graphics publication socket on this thread. */
    this->zmq_graphics_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    
    /* ZMQ: Bind. */
    this->zmq_graphics_publisher->bind("inproc://Graphics");
}

GraphicsPlugin::~GraphicsPlugin()
{      
}

/* Plugin: Multithreaded loop. */
void GraphicsPlugin::operator()()
{
    try
    {                
        /* ZMQ: Create framework subscription socket on this thread. */
        zmq::socket_t zmq_framework_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_framework_subscriber.connect("inproc://Framework");

        /* ZMQ: Suscribe to all messages. */
        zmq_framework_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        /* ZMQ: Create game subscription socket on this thread. */
        std::shared_ptr<zmq::socket_t> zmq_game_subscriber = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_game_subscriber->connect("inproc://Game");
        
        /* ZMQ: Suscribe to graphics messages. */
       zmq_game_subscriber->setsockopt(ZMQ_SUBSCRIBE, "Graphics", 0);
        
        /* EGL: Set configuration variables. */
        const EGLint egl_attributes[] =
        {
            EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
            EGL_BUFFER_SIZE,           32,
            EGL_RED_SIZE,              8,
            EGL_GREEN_SIZE,            8,
            EGL_BLUE_SIZE,             8,
            EGL_ALPHA_SIZE,            8,

            EGL_DEPTH_SIZE,            24,
            EGL_STENCIL_SIZE,          8,

            EGL_SAMPLE_BUFFERS,        0,
            EGL_SAMPLES,               0,

            EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,       EGL_OPENGL_ES2_BIT,

            EGL_NONE,
        };
        
        const EGLint egl_context_attributes[] =
        { 
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
        };

        /* EGL: Initialization. */
        auto egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);   

        EGLint egl_major = 0; 
        EGLint egl_minor = 0;
        eglInitialize(egl_display, &egl_major, &egl_minor);
        eglBindAPI(EGL_OPENGL_ES_API);

        /* EGL: Print debugging information. */        
        auto egl_info = std::string("-----EGL-----\n") + 
            "Vendor: " + eglQueryString(egl_display, EGL_VENDOR) + "\n" +
            "Version: " + eglQueryString(egl_display, EGL_VERSION) + "\n" +
            "Client APIs: " + eglQueryString(egl_display, EGL_CLIENT_APIS) + "\n" +
            "Extensions: " + eglQueryString(egl_display, EGL_EXTENSIONS);

        std::cout << egl_info << std::endl;

        /* EGL: Configuraion. */
        EGLint egl_num_configs = 0;
        EGLConfig egl_config;
        eglChooseConfig(egl_display, egl_attributes, &egl_config, 1, &egl_num_configs);

        /* EGL: Link to base::Window. */
        EGLint egl_format;
        eglGetConfigAttrib(egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
        auto egl_surface = eglCreateWindowSurface(egl_display, egl_config, this->base_window.get()->native_window(egl_format), NULL);  

        /* EGL: Context creation. */
        auto egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attributes);
        
        /* EGL: Make context current on this thread. */
        eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
        
        /* OGL: Render initialization. */
        Render ogl_render{this->base_window, zmq_game_subscriber};
        
        /* ZMQ: Send ready message. */
        {
            base::StringHash message("Ready");
            zmq::message_t zmq_message_send(message.Size());
            memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
            this->zmq_graphics_publisher->send(zmq_message_send);
        }
            
        /* ZMQ: Listen for start message. */
        for(;;)
        {
            zmq::message_t zmq_message;
            if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (base::StringHash("Start") == base::StringHash(zmq_message.data()))
                {
                    break;
                }
            }
        }
        
        /* Plugin: Loop. */
        std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock::now();
        for(;;)
        {            
            /* ZMQ: Listen for stop message. */
            {
                zmq::message_t zmq_message;
                if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    if (base::StringHash("Stop") == base::StringHash(zmq_message.data()))
                    {
                        break;
                    }
                }
            }
            
            /* Plugin: Timer*/
            auto newtime = std::chrono::high_resolution_clock::now();          
            auto deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 120000>>>(newtime - oldtime).count();
            oldtime = newtime;  
            
            if(this->base_window->poll())
            {
                /* OGL: Update & Draw. */
                ogl_render(deltatime);
            
                /* EGL: Swap buffers. */
                eglSwapBuffers(egl_display, egl_surface);
            }
        }
        /* EGL: Cleanup. */
        eglDestroyContext(egl_display, egl_context);
        eglDestroySurface(egl_display, egl_surface);
        eglTerminate(egl_display);
    }
    catch (...)
    {
        /* ZMQ: Send stop message. */
        base::StringHash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_graphics_publisher->send(zmq_message_send);

        std::rethrow_exception(std::current_exception());
        return;
    }
}
