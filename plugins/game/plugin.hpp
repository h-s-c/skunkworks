// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <memory>

#include <zmq.hpp>

class GamePlugin : public Plugin
{
  public:
    GamePlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~GamePlugin() override;  
    virtual void Loop() override;  
    
  private: 
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
};


