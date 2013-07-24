// Public Domain
#pragma once

#include "base/platform.hpp"
#include "base/system/window.hpp"

#include <functional>
#include <memory>

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
        std::function<std::unique_ptr<Plugin>(const std::shared_ptr<base::Window>&, const std::shared_ptr<zmq::context_t>&)> InitPlugin;
    };
}
