#include "plugins/game/entitymanager.hpp"
#include "plugins/common/entity.hpp"

#include <zeug/string_hash.hpp>
#include <zmq.hpp>
#include <msgpack.hpp>

#include <mutex>

int test = 0;
static std::once_flag flag;
static std::once_flag flag2;

EntityManager::EntityManager(const std::shared_ptr<zmq::socket_t> &zmq_game_publisher) : zmq_game_publisher(zmq_game_publisher)
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::operator()(bool start_game)
{
    /* Gamelogic: Create 3 sprites */
    std::call_once(flag, [&](){ 
        /* Topic. */
        {
            zeug::string_hash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            zeug::string_hash message("Create");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Entity data. */
        {
            Entity sprite{1, 0, 0, 0.4f, "assets/players/darksaber/sprite", "None"};

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{2, 100, 100, 0.4f, "assets/players/darksaber/sprite", "None"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{3, -100, -100, 0.4f, "assets/players/darksaber/sprite", "None"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* End of message. */
        {
            zeug::string_hash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    });
    
    if(!start_game)
    {
        return;
    }

    test++;
    /* Gamelogic: Update sprites */
    { 
        /* Topic. */
        {
            zeug::string_hash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            zeug::string_hash message("Update");
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
        std::call_once(flag2, [&](){
            Entity sprite;
            sprite = {2, 0, 0, 0.4f, "./../../assets/players/darksaber/sprite", "IdleRight"};

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        });
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
            zeug::string_hash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    }
}

