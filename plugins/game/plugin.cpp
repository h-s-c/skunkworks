// Public Domain
#include "plugins/game/plugin.hpp"
#include "base/platform.hpp"
#include "base/string/stringhash.hpp"
#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <zmq.hpp>
#include <msgpack.hpp>

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer(new GamePlugin(base_window, zmq_context));
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Game = { &InitPlugin};
}

GamePlugin::GamePlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{
}

GamePlugin::~GamePlugin()
{   
}

/* Plugin: Multithreaded loop. */
void GamePlugin::Loop()
{
    {
        /* ZMQ: Create general publication socket on this thread. */
        zmq::socket_t zmq_general_publisher (*this->zmq_context.get(), ZMQ_PUB);
        
        /* ZMQ: Bind. */
        zmq_general_publisher.bind("inproc://general");
        
        /* ZMQ: Create game publication socket on this thread. */
        zmq::socket_t zmq_game_publisher (*this->zmq_context.get(), ZMQ_PUB);
        
        /* ZMQ: Bind. */
        zmq_game_publisher.bind("inproc://game");
        
        /* ZMQ: Wait a bit for other plugins to etablish sockets. */
        std::chrono::milliseconds duration( 100 );
        std::this_thread::sleep_for( duration );
        
        /* ZMQ: Create input subscription socket on this thread. */
        zmq::socket_t zmq_input_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_input_subscriber.connect("inproc://input");

        /* ZMQ: Suscribe to all messages. */
        zmq_input_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        /* Gamelogic: Create sprite1 */
        { 
            /*std::chrono::milliseconds duration2( 1000 );
            std::this_thread::sleep_for( duration2 );*/
            /* Suscriber. */
            {
                base::StringHash message("Graphics");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Topic. */
            {
                base::StringHash message("Sprite");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Subtopic. */
            {
                base::StringHash message("Create");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Parameters. */
            {
                {
                    base::StringHash message("Id");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 1;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Path");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("./../../assets/players/darksaber/sprite");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionX");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 0;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionY");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 0;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Scale");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    float value = 0.2f;
                    msgpack::sbuffer sbuf;
                    msgpack::pack(&sbuf, value);
                    
                    zmq::message_t zmq_message( sbuf.size());
                    memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("State");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("IdleRight");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                base::StringHash message("Finish");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message);
            }
        }
        
        /* Gamelogic: Create sprite2 */
        { 
            /*std::chrono::milliseconds duration2( 1000 );
            std::this_thread::sleep_for( duration2 );*/
            /* Suscriber. */
            {
                base::StringHash message("Graphics");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Topic. */
            {
                base::StringHash message("Sprite");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Subtopic. */
            {
                base::StringHash message("Create");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Parameters. */
            {
                {
                    base::StringHash message("Id");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 2;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Path");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("./../../assets/players/darksaber/sprite");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionX");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 100;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionY");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 100;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Scale");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    float value = 0.2f;
                    msgpack::sbuffer sbuf;
                    msgpack::pack(&sbuf, value);
                    
                    zmq::message_t zmq_message( sbuf.size());
                    memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("State");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("WalkLeft");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                base::StringHash message("Finish");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message);
            }
        }
        
        /* Gamelogic: Create sprite3 */
        { 
            /* Suscriber. */
            {
                base::StringHash message("Graphics");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Topic. */
            {
                base::StringHash message("Sprite");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Subtopic. */
            {
                base::StringHash message("Create");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
            }
            /* Parameters. */
            {
                {
                    base::StringHash message("Id");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = 3;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Path");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("./../../assets/players/darksaber/sprite");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionX");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = -100;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("PositionY");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::int32_t value = -100;
                    zmq::message_t zmq_message(sizeof(std::int32_t));
                    memcpy(zmq_message.data(), &value, sizeof(std::int32_t));
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("Scale");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    float value = 0.2f;
                    msgpack::sbuffer sbuf;
                    msgpack::pack(&sbuf, value);
                    
                    zmq::message_t zmq_message( sbuf.size());
                    memcpy(zmq_message.data(), sbuf.data(), sbuf.size());
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                {
                    base::StringHash message("State");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
                {
                    std::string message("WalkRight");
                    zmq::message_t zmq_message(message.length());
                    memcpy(zmq_message.data(), message.c_str(), message.length()); 
                    zmq_game_publisher.send(zmq_message, ZMQ_SNDMORE);
                }
            }
            {
                base::StringHash message("Finish");
                zmq::message_t zmq_message(message.Size());
                memcpy(zmq_message.data(), message.Get(), message.Size()); 
                zmq_game_publisher.send(zmq_message);
            }
        }
    
        /* Plugin: Loop. */        
        for(;;)
        {            
            /* ZMQ: Listen. */
            zmq::message_t zmq_message;
            if (zmq_input_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                /* Topic */
                if (base::StringHash("Keyboard") == base::StringHash(zmq_message.data()))
                {
                    zmq_input_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("Esc") == base::StringHash(zmq_message.data()))
                    {
                        zmq_input_subscriber.recv(&zmq_message, 0);
                        if (base::StringHash("Finish") == base::StringHash(zmq_message.data()))
                        {
                        }
                        /* ZMQ: Send. */
                        base::StringHash message("Stop");
                        zmq::message_t zmq_message_send(message.Size());
                        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
                        zmq_general_publisher.send(zmq_message_send);
                        break;
                    }
                }
            }
        }
    }
}
