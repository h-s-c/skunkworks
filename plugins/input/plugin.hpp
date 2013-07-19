// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <memory>

#include <OIS/OIS.h>
#include <zmq.hpp>

class InputPlugin : public Plugin
{
  public:
    InputPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~InputPlugin() override;  
    virtual void operator()() override;  
    
  private: 
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_input_publisher;
};


