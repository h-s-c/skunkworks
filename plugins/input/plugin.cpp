// Public Domain
#include "plugins/input/plugin.hpp"
#include "base/platform.hpp"
#include "framework/plugin_api.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#define GLEW_MX
#include <SDL2/SDL.h>
#include <GL/glew.h>
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
    std::unique_ptr<Plugin> pointer(new InputPlugin);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Input = { &InitPlugin};
}

InputPlugin::InputPlugin() : params(), input_socket()
{
    this->params.zmq_context = zctx_new();
    this->input_socket = zsocket_new( this->params.zmq_context, ZMQ_PUB );
    if (zsocket_bind( this->input_socket, "inproc://input") == -1)
    {  
        std::runtime_error e("GraphicsInput could not bind to inproc://input");
        throw e;
    }
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(SDL_GetError());
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
        
    this->params.window = SDL_CheckError(SDL_CreateWindow(
        "Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN));
    this->params.gl_context = SDL_CheckError(SDL_GL_CreateContext(this->params.window ));
    
    SDL_GL_MakeCurrent( this->params.window, this->params.gl_context );    
    
    GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        throw std::runtime_error(
            (const char*)glewGetErrorString(res)
        );
    }
    glGetError();
    
    GLint major;
    GLint minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major); 
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL version: " << major << "." << minor << std::endl;
    
    SDL_GL_MakeCurrent( this->params.window, nullptr );
}

InputPlugin::~InputPlugin()
{
    SDL_GL_DeleteContext(this->params.gl_context);
    SDL_DestroyWindow(this->params.window);
    SDL_Quit();
    
    zctx_destroy(&this->params.zmq_context);
}


void InputPlugin::Loop()
{
    SDL_Event event;
    bool stop = false;
    while ( !stop )
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                stop = true;
                std::clog << "InputPlugin has send STOP signal" << std::endl;
                zstr_send( input_socket, "STOP" );
                break;
            }
        }
    }
}
