#pragma once

#include "plugins/common/entity.hpp"

#include <zmq.hpp>

class Step
{
  public:
    Step(const std::shared_ptr<zmq::socket_t> &zmq_physics_publisher, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber);
    ~Step();
    void operator()(double deltatime);
    
  private:
    std::vector<Entity> entities;
    std::shared_ptr<zmq::socket_t> zmq_physics_publisher;
    std::shared_ptr<zmq::socket_t> zmq_game_subscriber;
};
