// Public Domain
#pragma once

#include <memory>
#include <vector>

#include <zeug/window.hpp>
#include <zmq.hpp>

#include "plugins/graphics/sprite.hpp"

class Render
{
  public:
    Render(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber);
    void operator()(double deltatime);
    
  private:
    float akkumulator;
    std::vector<Sprite> sprites;
    
    std::shared_ptr<zeug::window> base_window;
    std::shared_ptr<zmq::socket_t> zmq_game_subscriber;
};
