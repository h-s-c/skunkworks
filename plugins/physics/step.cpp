#include "plugins/physics/step.hpp"

#include "base/string/stringhash.hpp"
#include "plugins/common/entity.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

Step::Step(const std::shared_ptr<zmq::socket_t> &zmq_physics_publisher, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber) : zmq_physics_publisher(zmq_physics_publisher), zmq_game_subscriber(zmq_game_subscriber)
{
}

Step::~Step()
{
}

void Step::operator()(double deltatime)
{
}

