// Public Domain

#include "framework/framework.hpp"
#include "framework/plugin_api.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
#include <utility>

#include <zeug/platform.hpp>
#include <zeug/detail/platform_macros.hpp>
#include <zeug/shared_lib.hpp>
#include <zeug/string_hash.hpp>
#include <zeug/window.hpp>

#include <zmq.hpp>

int RunFramework(EGLNativeWindowType window, const char* pakfile)
{
    try
    {
        Framework framework(window, pakfile);
        framework();
        return 0;
    }
    /* General: Catches exceptions from the mainthread as well as all propagated exceptions from other threads*/
    catch(const zmq::error_t& zmq_exception)
    {
       std::cerr << "ZMQ error: " << zmq_exception.what() << std::endl;
    }
    catch(const std::runtime_error& runtime_error)
    {
        std::cerr << "Runtime error: " << runtime_error.what() << std::endl;
    }
    catch (const std::exception& exception)
    {
        std::cerr  << "General exception: " <<exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr  << "Unhandled exception!" << std::endl;
    }
    return 1;
}
    
Framework::Framework(EGLNativeWindowType window, std::string pakfile)
{
    /* General: Print debugging information. */
    std::cout << std::string("-----Pakfile-----\nName: " +  pakfile) << std::endl;
    std::cout << zeug::platform::verbose() << std::endl;

    /* zeug::window: Initialization. */
    this->base_window = std::make_shared<zeug::window>(window);
    
    /* ZMQ: Initialization with 0 worker threads (we are using shared memory). */
    this->zmq_context = std::make_shared<zmq::context_t>(0);
    
    /* ZMQ: Create framework publication socket on this thread. */
    this->zmq_framework_publisher = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_PUB);
    
    /* ZMQ: Bind. */
    this->zmq_framework_publisher->bind("inproc://Framework");
    
    /* Plugins: Initialization.*/
    LoadPlugin("Game");
    LoadPlugin("Graphics");  
    LoadPlugin("Input");
    LoadPlugin("Physics");
}

Framework::~Framework()
{   
    /* Plugins: Cleanup. */
    plugins.clear();
    handles.clear();
}

void Framework::operator()()
{   
    /* Plugins: Start in their own thread. */
    for ( auto& plugin : this->plugins)
    {
        this->threads.push_back(std::thread([&]() {RunPlugin(std::move(plugin));}));
    }
    
    /* ZMQ: Wait for ready messages. */
    for( auto& subscriber : subscriptions)
    {
        while(this->base_window->poll())
        {
            zmq::message_t zmq_message;
            if (subscriber->recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (zeug::string_hash("Ready") == zeug::string_hash(zmq_message.data()))
                {
                    break;
                }
            }
        }
    }
    
    /* ZMQ: Send start message. */
    {
        zeug::string_hash message("Start");
        zmq::message_t zmq_message;
        memcpy(zmq_message.data(), message.Get(), message.Size()); 
        this->zmq_framework_publisher->send(zmq_message);
    }
    
    /* Framework: Loop. */
    auto stop = false;
    while(!stop)
    {
        if(!this->base_window->poll())
        {
            stop = true;
        }
        for( auto& subscriber : subscriptions)
        {
            zmq::message_t zmq_message;
            if (subscriber->recv(&zmq_message, ZMQ_NOBLOCK)) 
            {
                if (zeug::string_hash("Stop") == zeug::string_hash(zmq_message.data()))
                {
                    stop = true;
                    break;
                }
            }
        }
    }
    
    /* ZMQ: Send stop message. */
    {
        zeug::string_hash message("Stop");
        zmq::message_t zmq_message;
        memcpy(zmq_message.data(), message.Get(), message.Size()); 
        this->zmq_framework_publisher->send(zmq_message);
    }

    /*  Plugins: Wait for threads to finish. */
    for ( auto& thread : this->threads)
    {       
        thread.join();
    }
    
    /* Plugins: Rethrow propagated exceptions */
    for(const std::exception_ptr& exception : propagated_exceptions)
    {
        if(exception)
        {
            std::rethrow_exception(exception);
        }
    }
}

void Framework::LoadPlugin(std::string name)
{  
#if defined (PLATFORM_ANDROID)
    auto handle = std::make_unique<zeug::shared_lib>("/data/data/com.hsc.skunkworks/lib/", std::string("Plugin"+name));
#else
    auto handle = std::make_unique<zeug::shared_lib>(std::string("Plugin"+name));
#endif
    auto funcs = reinterpret_cast<PluginFuncs*>(handle.get()->symbol(name));
    
    handles.push_back(std::move(handle));
    plugins.push_back(std::move(funcs->InitPlugin(base_window, zmq_context)));
    
    std::this_thread::sleep_for(std::chrono::milliseconds( 10 ));
    std::string socket_name = "inproc://" + name;
    
    /* ZMQ: Create plugin specific subscription socket on this thread. */
    std::shared_ptr<zmq::socket_t> subscriber = std::make_shared<zmq::socket_t>(*this->zmq_context.get(), ZMQ_SUB);
    /* ZMQ: Connect. */
    subscriber->connect(socket_name.c_str());
    /* ZMQ: Suscribe to stop messages. */
    subscriber->setsockopt(ZMQ_SUBSCRIBE, "Stop", 0);
    
    subscriptions.push_back(std::move(subscriber));
}

void Framework::RunPlugin(std::unique_ptr<Plugin> plugin)
{  
    try
    {
        plugin->operator()();
        plugin.release();
    }
    /* Plugin: Catch plugin exceptions and propagate them to the mainthread*/
    catch (...)
    {
        std::lock_guard<std::mutex> lock(propagated_exceptions_mutex);
        propagated_exceptions.push_back(std::current_exception());
        return;
    }
}

