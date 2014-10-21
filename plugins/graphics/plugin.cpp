// Public Domain
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <zeug/platform.hpp>
#include <zeug/string_hash.hpp>
#include <zeug/window.hpp>
#include <zmq.hpp>

#include "framework/plugin_api.hpp"
#include "plugins/graphics/plugin.hpp"
#include "plugins/graphics/render.hpp"

extern "C" { 
 COMPILER_DLLEXPORT std::uint32_t NvOptimusEnablement = 0x00000001;
}

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer = std::make_unique<GraphicsPlugin>(base_window, zmq_context);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Graphics = { &InitPlugin};
}

GraphicsPlugin::GraphicsPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{      
    this->zmq_graphics_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    this->zmq_graphics_publisher->bind("inproc://Graphics");
}

GraphicsPlugin::~GraphicsPlugin()
{      
}

void GraphicsPlugin::operator()()
{
    try
    {                
        zmq::socket_t zmq_framework_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        zmq_framework_subscriber.connect("inproc://Framework");
        zmq_framework_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        std::shared_ptr<zmq::socket_t> zmq_game_subscriber = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_SUB);
        zmq_game_subscriber->connect("inproc://Game");
        zmq_game_subscriber->setsockopt(ZMQ_SUBSCRIBE, "Graphics", 0);

        const EGLint egl_attributes[] =
        {
            EGL_SURFACE_TYPE,             EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,      EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE,                       8,
            EGL_GREEN_SIZE,                  8,
            EGL_BLUE_SIZE,                     8,
            EGL_ALPHA_SIZE,                  8,
            EGL_DEPTH_SIZE,                  24,
            EGL_STENCIL_SIZE,                8,
            EGL_NONE
        };
        
        const EGLint egl_context_attributes[] =
        { 
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
        };

        auto egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);   
        eglInitialize(egl_display, 0, 0);
        eglBindAPI(EGL_OPENGL_ES_API);
    
        auto egl_info = std::string("-----EGL-----\n") + 
            "Vendor: " + eglQueryString(egl_display, EGL_VENDOR) + "\n" +
            "Version: " + eglQueryString(egl_display, EGL_VERSION) + "\n" +
            "Client APIs: " + eglQueryString(egl_display, EGL_CLIENT_APIS) + "\n" +
            "Extensions: " + eglQueryString(egl_display, EGL_EXTENSIONS);

        std::cout << egl_info << std::endl;

        EGLint egl_format = 0;
        EGLint egl_num_configs = 0;
        EGLConfig egl_config;
        eglChooseConfig(egl_display, egl_attributes, &egl_config, 1, &egl_num_configs);
        eglGetConfigAttrib(egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);

        auto egl_surface = eglCreateWindowSurface(egl_display, egl_config, this->base_window.get()->native_window(egl_format), NULL);  
        auto egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attributes);

        eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
        
        // Render class lifetime {}
        {
            Render ogl_render{this->base_window, zmq_game_subscriber};
            
            zeug::string_hash message("Ready");
            zmq::message_t zmq_message_send(message.Size());
            memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
            this->zmq_graphics_publisher->send(zmq_message_send);
            glClearColor(1, 1, 1, 1);
            for(;;)
            {
                zmq::message_t zmq_message;
                if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    if (zeug::string_hash("Start") == zeug::string_hash(zmq_message.data()))
                    {
                        break;
                    }
                }
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                eglSwapBuffers(egl_display, egl_surface);
            }
            
            std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock::now();
            for(;;)
            {            
                zmq::message_t zmq_message;
                if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    if (zeug::string_hash("Stop") == zeug::string_hash(zmq_message.data()))
                    {
                        break;
                    }
                }
                
                auto newtime = std::chrono::high_resolution_clock::now();          
                auto deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 120000>>>(newtime - oldtime).count();
                oldtime = newtime;  
                
                if(this->base_window->poll())
                {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    ogl_render(deltatime);
                    eglSwapBuffers(egl_display, egl_surface);
                }
            }
        }

        eglDestroyContext(egl_display, egl_context);
        eglDestroySurface(egl_display, egl_surface);
        eglTerminate(egl_display);
    }
    catch (...)
    {
        zeug::string_hash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_graphics_publisher->send(zmq_message_send);

        std::rethrow_exception(std::current_exception());
        return;
    }
}
