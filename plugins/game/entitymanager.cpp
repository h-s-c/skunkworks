#include "plugins/game/entitymanager.hpp"
#include "plugins/common/entity.hpp"

#include <zeug/stringhash.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>

#include <mutex>

int test = 0;
std::once_flag flag;

EntityManager::EntityManager(const std::shared_ptr<zmq::socket_t> &zmq_game_publisher) : zmq_game_publisher(zmq_game_publisher)
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::operator()()
{
    /* Gamelogic: Create 3 sprites */
    std::call_once(flag, [&](){ 
        /* Topic. */
        {
            zeug::stringhash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            zeug::stringhash message("Create");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Entity data. */
        {
            Entity sprite{1, 0, 0, 0.4f, "./../../assets/players/darksaber/sprite", "WalkRight"};

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{2, 100, 100, 0.4f, "./../../assets/players/darksaber/sprite", "IdleRight"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{3, -100, -100, 0.4f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* End of message. */
        {
            zeug::stringhash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    });
    
    test++;
    /* Gamelogic: Update sprites */
    { 
        /* Topic. */
        {
            zeug::stringhash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            zeug::stringhash message("Update");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Entity data. */
        {
            Entity sprite;
            if(test < 100)
            {
                sprite = {1, -200+test, -100, 0.4f, "./../../assets/players/darksaber/sprite", "WalkRight"};
            }
            else if(test < 200)
            {
                sprite = {1, -test, -100, 0.4f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            }
            else if(test == 200)
            {
                test = 0;
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite;
            if(test < 100)
            {
                sprite = {3, 200-test, 0, 0.4f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            }
            else if(test < 200)
            {
                sprite = {3, test, 0, 0.4f, "./../../assets/players/darksaber/sprite", "WalkRight"};
            }
            else if(test == 200)
            {
                test = 0;
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* End of message. */
        {
            zeug::stringhash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    }
}

