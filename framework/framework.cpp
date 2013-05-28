// Public Domain

#include "framework/framework.hpp"
#include "framework/plugin_api.hpp"
#include "base/system/info.hpp"
#include "base/system/library.hpp"
#include "base/system/window.hpp"

#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
#include <utility>

#include <zmq.hpp>
    
Framework::Framework()
{
    /* base::window: Initialization. */
    this->base_window = std::make_shared<base::Window>(800, 600, false);
    
    /* ZMQ: Initialization with 1 worker threads. */
    this->zmq_context = std::make_shared<zmq::context_t>(1);
    
    /* Plugins: Initialization.*/
    LoadPlugin("Graphics");  
    LoadPlugin("Input");
}

Framework::~Framework()
{   
    /* Plugins: Cleanup. */
    plugins.clear();
    
    for ( auto& handle : this->handles)
    {
        base::CloseLibrary(handle);
    }
}

void Framework::Loop()
{   
    /* Plugins: Start in their own thread. */
    for ( auto& plugin : this->plugins)
    {
        this->threads.push_back(std::thread([&]() {RunPlugin(std::move(plugin));}));
    }
    
    /* ZMQ: Wait a bit for other plugins to etablish sockets. */
    std::chrono::milliseconds dura( 2000 );
    std::this_thread::sleep_for( dura );
    
    /* ZMQ: Create input subscription socket on this thread. */
    zmq::socket_t zmq_input_subscriber(*this->zmq_context.get(), ZMQ_SUB);
    
    /* ZMQ: Connect. */
    zmq_input_subscriber.connect("inproc://input");

    /* ZMQ: Suscribe to all messages. */
    zmq_input_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    
    /* Framework: Loop. */
    for(;;)
    {
        /* ZMQ: Listen. */
        zmq::message_t zmq_message;
        if (zmq_input_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
        {
            if (std::string("STOP") == std::string(static_cast<char*>(zmq_message.data()), zmq_message.size()))
            {
                std::cout<< "ZMQ: Framework received STOP signal." << std::endl;
                break;
            }
        }
        std::this_thread::yield();
    }
    
    /*  Plugins: Wait for threads to finish. */
    for ( auto& thread : this->threads)
    {       
        thread.join();
    }
}

void Framework::LoadPlugin(std::string name)
{  
    auto handle = base::OpenLibrary(std::string("Plugin"+name), base::GetExecutableFilePath());
    auto funcs = reinterpret_cast<PluginFuncs*>(base::GetSymbol(handle, name ));
    
    handles.push_back(&handle);
    plugins.push_back(std::move(funcs->InitPlugin(base_window, zmq_context)));
}

void Framework::RunPlugin(std::unique_ptr<Plugin> plugin)
{  
    plugin.get()->Loop();
    plugin.release();
}

