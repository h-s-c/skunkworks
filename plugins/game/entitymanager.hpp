#pragma once

#include "plugins/common/entity.hpp"

#include <zmq.hpp>

class EntityManager
{
  public:
    EntityManager(const std::shared_ptr<zmq::socket_t> &zmq_game_publisher);
    void Update();
    
  private:
    std::vector<Entity> entities;
    std::shared_ptr<zmq::socket_t> zmq_game_publisher;
};
