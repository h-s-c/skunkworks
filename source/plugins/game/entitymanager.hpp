#pragma once

#include "plugins/common/entity.hpp"

#include <zmq.hpp>

class EntityManager
{
  public:
    EntityManager(const std::shared_ptr<zmq::socket_t> &zmq_game_publisher);
    ~EntityManager();
    void operator()(bool start_game);
    
  private:
    std::vector<Entity> entities;
    std::shared_ptr<zmq::socket_t> zmq_game_publisher;
};
