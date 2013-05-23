// Public Domain
#pragma once

#include <functional>
#include <memory>

#include <SDL2/SDL.h>
#include <czmq.h>

struct PluginParams
{
    SDL_Window* window;
    SDL_GLContext gl_context;
    zctx_t* zmq_context;
};

class Plugin
{
  public:
    Plugin() {};
    virtual void Loop() = 0;
    virtual void SetParameters(PluginParams* params) = 0;
    virtual PluginParams* GetParameters() = 0;
};

extern "C"
{    
    struct PluginFuncs 
    {
        std::function<std::unique_ptr<Plugin>(void)> InitPlugin;
    };
}
