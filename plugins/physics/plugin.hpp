// Public Domain
#pragma once
#include "framework/plugin_api.hpp"

#include <memory>

#include <platt/window.hpp>
#include <zmq.hpp>

class PhysicsPlugin : public Plugin
{
  public:
    PhysicsPlugin(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~PhysicsPlugin() override;  
    virtual void operator()() override;  
    
  private: 
    std::shared_ptr<platt::window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_physics_publisher;
};


