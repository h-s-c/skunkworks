// Public Domain

#include "framework/framework.hpp"
#include "framework/plugin_api.hpp"
#include "base/platform.hpp"
#include "base/system/info.hpp"
#include "base/system/library.hpp"

#include <iostream>
#include <thread>
#include <stdexcept>

#include <SDL2/SDL.h>

#define GLEW_MX
#include <GL/glew.h>

static thread_local GLEWContext* glew_context = nullptr;

GLEWContext* glewGetContext()
{
   if (glew_context == nullptr)
   {
        glew_context = new GLEWContext();
   }
   return glew_context;
}

namespace framework
{
int Run()
{
    try
    {
        Framework framework;
        framework.Run();
        return 0;
    }
    catch(std::runtime_error& sre)
    {
        std::cerr << "Runtime error: " << sre.what() << std::endl;
    }
    return 1;
}
}
    
Framework::Framework() : done(false)
{
    InitGL();
    LoadPlugin("Graphics");
}

void Framework::Run()
{      
    struct Parameters
    {
        SDL_GLContext context;
        SDL_Window* window;
    };
    Parameters params = {context, window};
    for ( auto &it : plugins)
    {
        it.pointer.get()->Start(&params);
    }
    
    SDL_Event event;
    while ( !done )
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                done = true;
            }
        }
    }
    
    for ( auto& it : plugins)
    {
        it.pointer.get()->Stop();        
        it.pointer.release();
        base::CloseLibrary(it.handle);
    }
    plugins.clear();
}

void Framework::LoadPlugin(std::string name)
{  
    void* handle = base::OpenLibrary(std::string("Plugin"+name), base::GetExecutableFilePath());
    struct framework::PluginFuncs* funcs = reinterpret_cast<framework::PluginFuncs*>(base::GetSymbol(handle, name ));
    
    if (funcs)
    {
        plugins.push_back({name, handle, std::move(funcs->GetPluginPtr())});
    }
}

void Framework::InitGL()
{   
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(SDL_GetError());
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
        
    window = SDL_CheckError(SDL_CreateWindow(
        "Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN));
    context = SDL_CheckError(SDL_GL_CreateContext( window ));
    
    glewExperimental = true;
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
    
    SDL_GL_MakeCurrent( window, nullptr );
}


Framework::~Framework()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
