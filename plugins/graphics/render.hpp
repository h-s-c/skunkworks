// Public Domain
#pragma once

#include "plugins/graphics/sprite.hpp"

#include <memory>
#include <vector>

#include <platt/window.hpp>

#include <zmq.hpp>

class TextureManager
{
  public:
    TextureManager() : slots(-1) {};
    std::uint32_t GetEmptySlot();
    
  private:
    std::uint32_t slots;
};

class Render
{
  public:
    Render(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber);
    void operator()(double deltatime);
    
  private:
    float akkumulator;
    std::vector<Sprite> sprites;
    std::shared_ptr<TextureManager> texturemanager;
    
    std::shared_ptr<platt::window> base_window;
    std::shared_ptr<zmq::socket_t> zmq_game_subscriber;
};
