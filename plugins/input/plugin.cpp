// Public Domain
#include "plugins/input/plugin.hpp"
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

#include <OIS/OIS.h>
#include <zmq.hpp>

std::unique_ptr<Plugin> InitPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
{
    std::unique_ptr<Plugin> pointer(new InputPlugin(base_window, zmq_context));
    return std::move(pointer);
}

extern "C" 
{
    COMPILER_DLLEXPORT struct PluginFuncs Input = { &InitPlugin};
}

InputPlugin::InputPlugin(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::context_t> &zmq_context)
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
        
        /* OIS: Initialization.*/
        OIS::ParamList pl;
        std::ostringstream wnd; 
        wnd << this->base_window.get()->GetNativeWindow();
        pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));
        //pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        //pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
        
        auto ois_manager = OIS::InputManager::createInputSystem(pl);

        /* OIS: Print debugging information. */
        std::cout << "OIS version: " << ois_manager->getVersionName() << std::endl;
        std::cout << "OIS manager: " << ois_manager->inputSystemName() << std::endl;
        std::cout << "OIS total keyboards: " << ois_manager->getNumberOfDevices(OIS::OISKeyboard) << std::endl;
        std::cout << "OIS total mice: " << ois_manager->getNumberOfDevices(OIS::OISMouse) << std::endl;
        std::cout << "OIS total joysticks: " << ois_manager->getNumberOfDevices(OIS::OISJoyStick) << std::endl;

        /* OIS: Keyboard initialization. */
        auto ois_keyboard = static_cast<OIS::Keyboard*>(ois_manager->createInputObject( OIS::OISKeyboard, false));
        
        /* Plugin: Loop. */        
        for(;;)
        {          
            /* ZMQ: Listen for stop signal. */
            zmq::message_t zmq_message;
            if (zmq_framework_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (base::StringHash("Stop") == base::StringHash(zmq_message.data()))
                {
                    break;
                }
            }
            
            /* OIS: Handle input */
            ois_keyboard->capture();
            if( ois_keyboard->isKeyDown( OIS::KC_ESCAPE )) 
            {
                /* Topic */
                {
                    base::StringHash message("Keyboard");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_input_publisher->send(zmq_message, ZMQ_SNDMORE);
                }
                /* Message */
                {
                    base::StringHash message("Esc");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_input_publisher->send(zmq_message, ZMQ_SNDMORE);
                }
                /* End of message. */
                {
                    base::StringHash message("Finish");
                    zmq::message_t zmq_message(message.Size());
                    memcpy(zmq_message.data(), message.Get(), message.Size()); 
                    zmq_input_publisher->send(zmq_message);
                }
            }
        }
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(const OIS::Exception& ois_exception )
    {
        /* ZMQ: Send stop message. */
        base::StringHash message("Stop");
        zmq::message_t zmq_message_send(message.Size());
        memcpy(zmq_message_send.data(), message.Get(), message.Size()); 
        this->zmq_input_publisher->send(zmq_message_send);
        
        throw std::runtime_error(std::string(" OIS - ") + ois_exception.eText);
        return;
    }
}
