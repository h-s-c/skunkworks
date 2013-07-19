// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"
#include "plugins/game/entitymanager.hpp"

#include <memory>

#include <zmq.hpp>

class GamePlugin : public Plugin
{
  public:
    GamePlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~GamePlugin() override;
    void operator()() override;
    
  private: 
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_game_publisher;
};
