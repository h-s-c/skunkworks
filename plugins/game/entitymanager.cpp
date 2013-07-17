#include "plugins/game/entitymanager.hpp"

#include "base/string/stringhash.hpp"
#include "plugins/common/entity.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

int test = 0;

EntityManager::EntityManager(const std::shared_ptr<zmq::socket_t> &zmq_game_publisher) : zmq_game_publisher(zmq_game_publisher)
{
    /* Gamelogic: Create 3 sprites */
    { 
        /* Topic. */
        {
            base::StringHash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            base::StringHash message("Create");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Entity data. */
        {
            Entity sprite{1, 0, 0, 0.2f, "./../../assets/players/darksaber/sprite", "WalkRight"};

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{2, 100, 100, 0.2f, "./../../assets/players/darksaber/sprite", "IdleRight"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite{3, -100, -100, 0.2f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            
            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* End of message. */
        {
            base::StringHash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    }
}

void EntityManager::Update()
{
    test++;
    /* Gamelogic: Update sprites */
    { 
        /* Topic. */
        {
            base::StringHash message("Sprite");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Subtopic. */
        {
            base::StringHash message("Update");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* Entity data. */
        {
            Entity sprite;
            if(test < 100)
            {
                sprite = {1, -200+test, -100, 0.2f, "./../../assets/players/darksaber/sprite", "WalkRight"};
            }
            else if(test < 200)
            {
                sprite = {1, -test, -100, 0.2f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            }
            else if(test == 200)
            {
                test = 0;
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        {
            Entity sprite;
            if(test < 100)
            {
                sprite = {3, 200-test, 0, 0.2f, "./../../assets/players/darksaber/sprite", "WalkLeft"};
            }
            else if(test < 200)
            {
                sprite = {3, test, 0, 0.2f, "./../../assets/players/darksaber/sprite", "WalkRight"};
            }
            else if(test == 200)
            {
                test = 0;
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(&sbuf, sprite);
            
            this->entities.push_back(sprite);
            
            zmq::message_t zmq_message( sbuf.size());
            memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
            zmq_game_publisher->send(zmq_message, ZMQ_SNDMORE);
        }
        /* End of message. */
        {
            base::StringHash message("Finish");
            zmq::message_t zmq_message(message.Size());
            memcpy(zmq_message.data(), message.Get(), message.Size()); 
            zmq_game_publisher->send(zmq_message);
        }
    }
}

