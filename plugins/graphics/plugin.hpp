// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <memory>

#include <zmq.hpp>

class GraphicsPlugin : public Plugin
{
  public:
    GraphicsPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~GraphicsPlugin() override;   
    virtual void operator()() override;   
    
  private: 
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
};


