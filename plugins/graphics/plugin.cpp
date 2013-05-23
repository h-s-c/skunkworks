// Public Domain
#include "plugins/graphics/plugin.hpp"
#include "plugins/graphics/render.hpp"
#include "base/platform.hpp"
#include "framework/plugin_api.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#define GLEW_MX
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <oglplus/error.hpp>
#include <czmq.h>

static thread_local GLEWContext* glew_context = nullptr;

GLEWContext* glewGetContext()
{
   if (glew_context == nullptr)
   {
        glew_context = new GLEWContext();
   }
   return glew_context;
}

std::unique_ptr<Plugin> InitPlugin()
{
    std::unique_ptr<Plugin> pointer(new GraphicsPlugin);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Graphics = { &InitPlugin};
}

void GraphicsPlugin::Loop()
{      
    void * graphics_socket = zsocket_new( this->params.zmq_context, ZMQ_SUB );
    if (!graphics_socket)
    {  
        std::runtime_error e("GraphicsPlugin could not create socket for inproc://input");
        throw e;
    }
    
    if (zsocket_connect( graphics_socket, "inproc://input" ) == -1)
    {  
        std::runtime_error e("GraphicsPlugin could not connect to inproc://input");
        throw e;
    }
    
    zsocket_set_subscribe(graphics_socket, (char*)"");
    
    SDL_GL_MakeCurrent( this->params.window, this->params.gl_context );
    SDL_GL_SetSwapInterval(1);
    
    GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        throw std::runtime_error(
            (const char*)glewGetErrorString(res)
        );
    }
    glGetError();
    
    try
    {        
        Render render;
        for(;;)
        {
            char *message = zstr_recv_nowait(graphics_socket);
            if (message ) 
            {
                if (strcmp (message , "STOP") == 0)
                {
                    std::clog << "GraphicsPlugin received STOP signal" << std::endl;
                    free (message);
                    break;
                }
            }
            free (message);
            
            render.Display();
            SDL_GL_SwapWindow(this->params.window);
        }
    }
    catch(oglplus::Error& err)
    {
        std::cerr <<
            "Error (in " << err.GLSymbol() << ", " <<
            err.ClassName() << ": '" <<
            err.ObjectDescription() << "'): " <<
            err.what() <<
            " [" << err.File() << ":" << err.Line() << "] ";
        std::cerr << std::endl;
        err.Cleanup();
    }
}
