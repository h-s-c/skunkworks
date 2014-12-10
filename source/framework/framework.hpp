// Public Domain
#pragma once
#include "framework/plugin_api.hpp"

#include <zeug/window.hpp>
#include <zeug/shared_lib.hpp>

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
  
    std::shared_ptr<zeug::window> base_window;
    std::shared_ptr<zmq::context_t> zmq_context;
    std::shared_ptr<zmq::socket_t> zmq_framework_publisher;
    
    std::vector<std::unique_ptr<zeug::shared_lib>> handles;
    std::vector<std::unique_ptr<Plugin>> plugins;
    std::vector<std::thread> threads;
    
    std::vector<std::shared_ptr<zmq::socket_t>> subscriptions;
    
    std::vector<std::exception_ptr> propagated_exceptions;
    std::mutex propagated_exceptions_mutex;
};
