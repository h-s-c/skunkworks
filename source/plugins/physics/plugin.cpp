// Public Domain
#include "plugins/physics/plugin.hpp"
#include "framework/plugin_api.hpp"
#include "plugins/physics/step.hpp"

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
    std::unique_ptr<Plugin> pointer = std::make_unique<PhysicsPlugin>(base_window, zmq_context);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Physics = { &InitPlugin};
}

PhysicsPlugin::PhysicsPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{
    /* ZMQ: Create physics publication socket on this thread. */
    this->zmq_physics_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    
    /* ZMQ: Bind. */
    this->zmq_physics_publisher->bind("inproc://Physics");
}

PhysicsPlugin::~PhysicsPlugin()
{   
}

/* Plugin: Multithreaded loop. */
void PhysicsPlugin::operator()()
{
    try
    {
        /* ZMQ: Create game subscription socket on this thread. */
        zmq::socket_t zmq_framework_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_framework_subscriber.connect("inproc://Framework");

        /* ZMQ: Suscribe to all messages. */
        zmq_framework_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    
        /* ZMQ: Create game subscription socket on this thread. */
        std::shared_ptr<zmq::socket_t> zmq_game_subscriber = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_game_subscriber->connect("inproc://Game");

        /* ZMQ: Suscribe to physics messages. */
        zmq_game_subscriber->setsockopt(ZMQ_SUBSCRIBE, "Physics", 0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds( 10 ));
        
        /* Physics: Initialization. */
        Step step{this->zmq_physics_publisher, zmq_game_subscriber};
        
        /* ZMQ: Send ready message. */
        {
            zeug::string_hash message("Ready");
            zmq::message_t zmq_message_send(message.Size());
            memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
            this->zmq_physics_publisher->send(zmq_message_send);
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
        
        /* Plugin: Loop. */
        std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock::now();
        
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
            
            /* Plugin: Timer*/
            auto newtime = std::chrono::high_resolution_clock::now();          
            auto deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 120000>>>(newtime - oldtime).count();
            oldtime = newtime;  
            
            /* Physics: Step. */
            step(deltatime);
        }
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(...)
    {
        /* ZMQ: Send stop message. */
        zeug::string_hash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_physics_publisher->send(zmq_message_send);
        
        std::rethrow_exception(std::current_exception());
        return;
    }
}
