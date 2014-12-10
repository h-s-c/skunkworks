// Public Domain
#include "framework/plugin_api.hpp"
#include "plugins/game/plugin.hpp"
#include "plugins/game/entitymanager.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <zeug/platform.hpp>
#include <zeug/string_hash.hpp>
#include <zeug/window.hpp>
#include <zmq.hpp>

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer = std::make_unique<GamePlugin>(base_window, zmq_context);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Game = { &InitPlugin};
}

GamePlugin::GamePlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{
    /* ZMQ: Create game publication socket on this thread. */
    this->zmq_game_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    
    /* ZMQ: Bind. */
    this->zmq_game_publisher->bind("inproc://Game");
}

GamePlugin::~GamePlugin()
{   
}

/* Plugin: Multithreaded loop. */
void GamePlugin::operator()()
{
    try
    {                
        /* ZMQ: Create framework subscription socket on this thread. */
        zmq::socket_t zmq_framework_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_framework_subscriber.connect("inproc://Framework");

        /* ZMQ: Suscribe to all messages. */
        zmq_framework_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        /* ZMQ: Create input subscription socket on this thread. */
        zmq::socket_t zmq_input_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_input_subscriber.connect("inproc://Input");

        /* ZMQ: Suscribe to all messages. */
        zmq_input_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        EntityManager entitymanager{zmq_game_publisher};
        
        /* ZMQ: Send ready message. */
        {
            zeug::string_hash message("Ready");
            zmq::message_t zmq_message_send(message.Size());
            memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
            this->zmq_game_publisher->send(zmq_message_send);
        }
        
        /* ZMQ: Listen for start message. */
        for(;;)
        {
            zmq::message_t zmq_message;
            if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (zeug::string_hash("Start") == zeug::string_hash(zmq_message.data()))
                {
                    break;
                }
            }
        }
        
        bool start_game = false;

        /* Plugin: Loop. */        
        for(;;)
        {
            /* ZMQ: Listen for stop message. */
            {
                zmq::message_t zmq_message;
                if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    if (zeug::string_hash("Stop") == zeug::string_hash(zmq_message.data()))
                    {
                        break;
                    }
                }
            }
            /* ZMQ: Listen. */
            {
                zmq::message_t zmq_message;
                if (zmq_input_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    /* Topic */
                    if (zeug::string_hash("Keyboard") == zeug::string_hash(zmq_message.data()))
                    {
                        /* Message */
                        zmq_message.rebuild();
                        zmq_input_subscriber.recv(&zmq_message, 0);
                        if (zeug::string_hash("Esc") == zeug::string_hash(zmq_message.data()))
                        {
                            /* ZMQ: Send stop message. */
                            zeug::string_hash message("Stop");
                            zmq_message.rebuild();
                            memcpy(zmq_message.data(), message.Get(), message.Size()); 
                            this->zmq_game_publisher->send(zmq_message);
                        }
                        else if (zeug::string_hash("Enter") == zeug::string_hash(zmq_message.data()))
                        {
                            start_game = true;
                        }
                        /* End of message. */
                        zmq_message.rebuild();
                        zmq_input_subscriber.recv(&zmq_message, 0);
                        if (zeug::string_hash("Finish") == zeug::string_hash(zmq_message.data()))
                        {
                        }
                    }
                }
            }
            
            entitymanager(start_game);
        }
    }
    catch (...)
    {
        /* ZMQ: Send stop message. */
        zeug::string_hash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_game_publisher->send(zmq_message_send);
        
        std::rethrow_exception(std::current_exception());
        return;
    }
}
