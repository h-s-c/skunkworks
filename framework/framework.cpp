// Public Domain

#include "framework/framework.hpp"
#include "framework/plugin_api.hpp"
#include "base/string/stringhash.hpp"
#include "base/system/info.hpp"
#include "base/system/library.hpp"
#include "base/system/window.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
#include <utility>

#include <zmq.hpp>

#ifdef __cplusplus  
extern "C" 
{
#endif

int RunFramework()
{
    try
    {
        Framework framework;
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

#ifdef __cplusplus  
}
#endif
    
Framework::Framework()
{
    /* base::window: Initialization. */
    this->base_window = std::make_shared<base::Window>(800, 600, false);
    
    /* ZMQ: Initialization with 1 worker threads. */
    this->zmq_context = std::make_shared<zmq::context_t>(1);
    
    /* Plugins: Initialization.*/
    LoadPlugin("Graphics");  
    LoadPlugin("Input");
    LoadPlugin("Game");
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

void Framework::operator()()
{   
    /* Plugins: Start in their own thread. */
    for ( auto& plugin : this->plugins)
    {
        this->threads.push_back(std::thread([&]() {RunPlugin(std::move(plugin));}));
    }
    
    /* ZMQ: Wait a bit for other plugins to etablish sockets. */
    std::chrono::milliseconds duration( 100 );
    std::this_thread::sleep_for( duration );
    
    /* ZMQ: Create general subscription socket on this thread. */
    zmq::socket_t zmq_general_subscriber(*this->zmq_context.get(), ZMQ_SUB);
    
    /* ZMQ: Connect. */
    zmq_general_subscriber.connect("inproc://general");

    /* ZMQ: Suscribe to all messages. */
    zmq_general_subscriber.setsockopt(ZMQ_SUBSCRIBE, "stop", 0);
    
    /* Framework: Loop. */
    for(;;)
    {
        std::this_thread::yield();
    
        zmq::message_t zmq_message;
        if (zmq_general_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
        {
            if (base::StringHash("Stop") == base::StringHash(zmq_message.data()))
            {
                break;
            }
        }
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
    auto handle = base::OpenLibrary(std::string("Plugin"+name), base::GetExecutableFilePath());
    auto funcs = reinterpret_cast<PluginFuncs*>(base::GetSymbol(handle, name ));
    
    handles.push_back(&handle);
    plugins.push_back(std::move(funcs->InitPlugin(base_window, zmq_context)));
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

