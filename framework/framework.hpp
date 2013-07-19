// Public Domain
#pragma once

#include "base/system/window.hpp"
#include "framework/plugin_api.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <zmq.hpp>

class Framework
{
  public:
    Framework();
    ~Framework();  
    void operator()();
    
  private:   
    void LoadPlugin(std::string name);
    void RunPlugin(std::unique_ptr<Plugin> plugin);
  
    std::shared_ptr<base::Window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    
    std::vector<void*> handles; 
    std::vector<std::unique_ptr<Plugin>> plugins;
    std::vector<std::thread> threads;
    
    std::vector<std::shared_ptr<zmq::socket_t>> subscriptions;
    
    std::vector<std::exception_ptr> propagated_exceptions;
    std::mutex propagated_exceptions_mutex;
};
