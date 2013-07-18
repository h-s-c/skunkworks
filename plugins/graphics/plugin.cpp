// Public Domain
#include "plugins/graphics/plugin.hpp"
#include "base/platform.hpp"
#include "base/string/stringhash.hpp"
#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"
#include "plugins/graphics/render.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <eglplus/egl.hpp>
#include <eglplus/all.hpp>
#include <oglplus/gl.hpp>
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
void GraphicsPlugin::operator()()
{
    try
    {
        /* ZMQ: Wait a bit for other plugins to etablish sockets. */
        std::chrono::milliseconds duration( 100 );
        std::this_thread::sleep_for( duration );
        
        /* ZMQ: Create general subscription socket on this thread. */
        zmq::socket_t zmq_general_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_general_subscriber.connect("inproc://general");

        /* ZMQ: Suscribe to stop messages. */
        zmq_general_subscriber.setsockopt(ZMQ_SUBSCRIBE, "Stop", 0);
        
        /* ZMQ: Create game subscription socket on this thread. */
        std::shared_ptr<zmq::socket_t> zmq_game_subscriber = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_game_subscriber->connect("inproc://game");
        
        /* ZMQ: Suscribe to all messages. */
       zmq_game_subscriber->setsockopt(ZMQ_SUBSCRIBE, "Graphics", 0);
        
        /* EGL: Initialization. */
        eglplus::Display display;
        eglplus::LibEGL egl(display);

        /* EGL: Configuraion. */
        eglplus::Configs configs(
            display,
            eglplus::ConfigAttribs()
                .Add(eglplus::ConfigAttrib::RedSize, 8)
                .Add(eglplus::ConfigAttrib::GreenSize, 8)
                .Add(eglplus::ConfigAttrib::BlueSize, 8)
                .Add(eglplus::ConfigAttrib::DepthSize, 24)
                .Add(eglplus::ConfigAttrib::StencilSize, 8)
                .Add(eglplus::ColorBufferType::RGBBuffer)
                .Add(eglplus::RenderableTypeBit::OpenGL)
                .Add(eglplus::SurfaceTypeBit::Window)
                .Get()
        );

        eglplus::Config config = configs.First();

        /* EGL: Link to base::Window. */
        eglplus::Surface surface = eglplus::Surface::Window(
            display,
            config,
            this->base_window.get()->GetNativeWindow(0),
            eglplus::SurfaceAttribs().Get()
        );

        /* EGL: Context creation. */
        eglplus::BindAPI(eglplus::RenderingAPI::OpenGL);
        eglplus::Context context(
            display,
            config,
            eglplus::ContextAttribs()
                .Add(eglplus::ContextAttrib::ClientVersion, 3)
                .Add(eglplus::ContextAttrib::MinorVersion, 1)
                .Add(eglplus::ContextFlag::OpenGLRobustAccess)
                .Add(eglplus::ContextFlag::OpenGLDebug)
                .Add(eglplus::ContextFlag::OpenGLForwardCompatible)
                .Add(eglplus::OpenGLProfileBit::Core)
                .Add(eglplus::OpenGLResetNotificationStrategy::NoResetNotification)
                .Get()
        );

        /* EGL: Make context current on this thread. */
        context.MakeCurrent(surface);

        /* OGL: Initialization. */
        oglplus::GLAPIInitializer api_init;
        
        /* OGL: Print version information */
        GLint gl_major = 0; 
        GLint gl_minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
        glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
        std::cout << "GL version: " << gl_major << "." << gl_minor << std::endl;
        
        /* OGL: Render initialization. */
        Render ogl_render{zmq_game_subscriber};
        
        /* Plugin: Loop. */
        std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock::now();
        
        for(;;)
        {            
            /* ZMQ: Listen for stop signal. */
            zmq::message_t zmq_message;
            if (zmq_general_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (base::StringHash("Stop") == base::StringHash(zmq_message.data()))
                {
                    break;
                }
            }
            
            /* Plugin: Timer*/
            auto newtime = std::chrono::high_resolution_clock::now();          
            auto deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 120000>>>(newtime - oldtime).count();
            oldtime = newtime;  
            
            /* OGL: Update & Draw. */
            ogl_render(deltatime);
        
            /* EGL: Swap buffers. */
            surface.SwapBuffers();
        }
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(const oglplus::Error& err)
    {
        std::stringstream error_string; 
        error_string << "OGLPlus error (in " << 
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
    catch(const eglplus::Error& err)
    {
        std::stringstream error_string; 
        error_string << "EGLPlus error (in " << 
            err.EGLSymbol() << ", " <<
            err.ClassName() << ": '" <<
            err.what() << " [" << 
            err.File() << ":" << 
            err.Line() << "] " << 
            std::endl;
        err.Cleanup();   
        throw std::runtime_error(error_string.str());
    }
}
