// Public Domain
#include "plugins/input/plugin.hpp"
#include "base/platform.hpp"
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
}

InputPlugin::~InputPlugin()
{   
}

/* Plugin: Multithreaded loop. */
void InputPlugin::Loop()
{
    try
    {
        /* ZMQ: Create input publication socket on this thread. */
        zmq::socket_t zmq_input_publisher (*this->zmq_context.get(), ZMQ_PUB);
        
        /* ZMQ: Bind. */
        zmq_input_publisher.bind("inproc://input");
        
        /* OIS: Initialization.*/
        OIS::ParamList pl;
        std::ostringstream wnd; 
        wnd << this->base_window.get()->GetNativeWindow();
        pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));
        
        auto ois_manager = OIS::InputManager::createInputSystem(pl);
        ois_manager->enableAddOnFactory(OIS::InputManager::AddOn_All);

        /* OIS: Print debugging information. */
        std::cout << "OIS version: " << ois_manager->getVersionNumber() << std::endl;
        std::cout << "OIS release name: " << ois_manager->getVersionName() << std::endl;
        std::cout << "OIS manager: " << ois_manager->inputSystemName() << std::endl;
        std::cout << "OIS total keyboards: " << ois_manager->getNumberOfDevices(OIS::OISKeyboard) << std::endl;
        std::cout << "OIS total mice: " << ois_manager->getNumberOfDevices(OIS::OISMouse) << std::endl;
        std::cout << "OIS total joysticks: " << ois_manager->getNumberOfDevices(OIS::OISJoyStick) << std::endl;

        /* OIS: Keyboard initialization. */
        auto ois_keyboard = static_cast<OIS::Keyboard*>(ois_manager->createInputObject( OIS::OISKeyboard, false));
        
        /* Plugin: Loop. */        
        for(;;)
        {            
            /* OIS: Handle input */
            ois_keyboard->capture();
            if( ois_keyboard->isKeyDown( OIS::KC_ESCAPE )) 
            {
                /* ZMQ: Send. */
                std::string message = "STOP";
                zmq::message_t zmq_message(message.size());
                memcpy(zmq_message.data(), message.data(), message.size()); 
                zmq_input_publisher.send(zmq_message);
                std::cout<< "ZMQ: InputPlugin send STOP signal." << std::endl;
                break;
            }
        }
    }
    /* Plugin: Catch plugin specific exceptions and rethrow them as runtime error*/
    catch(const OIS::Exception& ois_exception )
    {
        throw std::runtime_error(std::string(" OIS - ") + ois_exception.eText);
    }
}
