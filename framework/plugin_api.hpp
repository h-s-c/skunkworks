// Public Domain
#pragma once

#include <functional>
#include <memory>

#include <platt/window.hpp>

#include <zmq.hpp>

class Plugin
{
  public:
    virtual ~Plugin() {};   
    virtual void operator()() = 0;
};

extern "C"
{    
    struct PluginFuncs 
    {
        std::function<std::unique_ptr<Plugin>(const std::shared_ptr<platt::window>&, const std::shared_ptr<zmq::context_t>&)> InitPlugin;
    };
}
