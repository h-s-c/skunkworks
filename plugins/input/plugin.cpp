// Public Domain
#include "framework/plugin_api.hpp"
#include "plugins/input/plugin.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <zeug/platform.hpp>
#include <zeug/stringhash.hpp>
#include <zeug/window.hpp>
#include <OIS/OIS.h>
#include <zmq.hpp>

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer = std::make_unique<InputPlugin>(base_window, zmq_context);
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Input = { &InitPlugin};
}

InputPlugin::InputPlugin(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
    : base_window(base_window), zmq_context(zmq_context)
{
    /* ZMQ: Create input publication socket on this thread. */
    this->zmq_input_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    
    /* ZMQ: Bind. */
    this->zmq_input_publisher->bind("inproc://Input");
}

InputPlugin::~InputPlugin()
{   
}

/* Plugin: Multithreaded loop. */
void InputPlugin::operator()()
{
    try
    {        
        /* ZMQ: Create framework subscription socket on this thread. */
        zmq::socket_t zmq_framework_subscriber (*this->zmq_context.get(), ZMQ_SUB);
        
        /* ZMQ: Connect. */
        zmq_framework_subscriber.connect("inproc://Framework");

        /* ZMQ: Suscribe to all messages. */
        zmq_framework_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds( 10 ));
        
        /* OIS: Initialization.*/
        OIS::ParamList pl;
        std::ostringstream wnd; 
        wnd << this->base_window.get()->native_window();
        pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));

        
        auto ois_manager = OIS::InputManager::createInputSystem(pl);

        /* OIS: Print debugging information. */
        auto ois_info = std::string("-----OIS-----\n") + 
            "Version: " + ois_manager->getVersionName() + "\n" +
            "Manager: " + ois_manager->inputSystemName() + "\n" +
            "Total keyboards: " + std::to_string(ois_manager->getNumberOfDevices(OIS::OISKeyboard)) + "\n" +
            "Total mice: " + std::to_string(ois_manager->getNumberOfDevices(OIS::OISMouse)) + "\n" +
            "Total gamepads: " + "-" + "\n" +
            "Total joysticks: " + std::to_string(ois_manager->getNumberOfDevices(OIS::OISJoyStick));

        std::cout << ois_info << std::endl;

        /* OIS: Keyboard initialization. */
        auto ois_keyboard = static_cast<OIS::Keyboard*>(ois_manager->createInputObject( OIS::OISKeyboard, false));
        
        /* ZMQ: Send ready message. */
        {
            zeug::stringhash message("Ready");
            zmq::message_t zmq_message_send(message.Size());
            memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
            this->zmq_input_publisher->send(zmq_message_send);
        }
        
        /* ZMQ: Listen for start message. */
        for(;;)
        {
            zmq::message_t zmq_message;
            if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (zeug::stringhash("Start") == zeug::stringhash(zmq_message.data()))
                {
                    break;
                }
            }
        }
        
        /* Plugin: Loop. */        
        for(;;)
        {          
            /* ZMQ: Listen for stop message. */
            {
                zmq::message_t zmq_message;
                if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
                {
                    if (zeug::stringhash("Stop") == zeug::stringhash(zmq_message.data()))
                    {
                        break;
                    }
                }
            }
            
            if (this->base_window->poll())
            {
                /* OIS: Handle input */
                ois_keyboard->capture();
                if( ois_keyboard->isKeyDown( OIS::KC_ESCAPE )) 
                {
                    /* Topic */
                    {
                        zeug::stringhash message("Keyboard");
                        zmq::message_t zmq_message(message.Size());
                        memcpy(zmq_message.data(), message.Get(), message.Size()); 
                        zmq_input_publisher->send(zmq_message, ZMQ_SNDMORE);
                    }
                    /* Message */
                    {
                        zeug::stringhash message("Esc");
                        zmq::message_t zmq_message(message.Size());
                        memcpy(zmq_message.data(), message.Get(), message.Size()); 
                        zmq_input_publisher->send(zmq_message, ZMQ_SNDMORE);
                    }
                    /* End of message. */
                    {
                        zeug::stringhash message("Finish");
                        zmq::message_t zmq_message(message.Size());
                        memcpy(zmq_message.data(), message.Get(), message.Size()); 
                        zmq_input_publisher->send(zmq_message);
                    }
                }
            }
        }
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(const OIS::Exception& ois_exception )
    {
        /* ZMQ: Send stop message. */
        zeug::stringhash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_input_publisher->send(zmq_message_send);
        
        throw std::runtime_error(std::string(" OIS - ") + ois_exception.eText);
        return;
    }
}
