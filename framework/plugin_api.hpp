// Public Domain
#pragma once

#include <functional>
#include <memory>

#include <SDL2/SDL.h>

namespace framework
{

class Plugin
{
  public:
    Plugin() {};
    virtual void Start(void * parameters) = 0;
    virtual void Stop() = 0;
};

extern "C"
{    
    struct PluginFuncs 
    {
        std::function<std::unique_ptr<Plugin>(void)> GetPluginPtr;
    };
}

}
