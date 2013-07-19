// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <memory>

#include <zmq.hpp>

class PhysicsPlugin : public Plugin
{
  public:
    PhysicsPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~PhysicsPlugin() override;  
    virtual void operator()() override;  
    
  private: 
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_physics_publisher;
};


