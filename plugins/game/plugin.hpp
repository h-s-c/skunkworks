// Public Domain
#pragma once

#include "framework/plugin_api.hpp"
#include "plugins/game/entitymanager.hpp"

#include <memory>

#include <platt/window.hpp>
#include <zmq.hpp>

class GamePlugin : public Plugin
{
  public:
    GamePlugin(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context);
    virtual ~GamePlugin() override;
    void operator()() override;
    
  private: 
    std::shared_ptr<platt::window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::shared_ptr<zmq::socket_t> zmq_game_publisher;
};
