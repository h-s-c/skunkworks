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
    SDL_GL_MakeCurrent( this->params.window, this->params.context );
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
            render.Display();
            SDL_GL_SwapWindow(this->params.window);
        }
        SDL_GL_MakeCurrent( this->params.window, nullptr );
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
