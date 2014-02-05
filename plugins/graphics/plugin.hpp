// Public Domain
#pragma once

#include <memory>

#include <platt/window.hpp>
#include <zmq.hpp>

#include "framework/plugin_api.hpp"

class GraphicsPlugin : public Plugin
{
  public:
    GraphicsPlugin(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~GraphicsPlugin() override;   
    virtual void operator()() override;   
    
  private: 
    std::shared_ptr<platt::window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_graphics_publisher;
};


