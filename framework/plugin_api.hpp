// Public Domain
#pragma once

#include <functional>
#include <memory>

#include <SDL2/SDL.h>

struct PluginParams
{
    SDL_Window* window;
    SDL_GLContext context;
};

class Plugin
{
  public:
    Plugin() {};
    virtual void Loop() = 0;
    virtual void SetParameters(PluginParams* params) = 0;
    virtual PluginParams* GetParameters() = 0;
    
    /*virtual void SetContext(SDL_GLContext context) = 0;
    virtual void SetWindow(SDL_Window* window) = 0;
    virtual SDL_GLContext GetContext() = 0;
    virtual SDL_Window* GetWindow() = 0;*/
};

extern "C"
{    
    struct PluginFuncs 
    {
        std::function<std::unique_ptr<Plugin>(void)> InitPlugin;
    };
}
