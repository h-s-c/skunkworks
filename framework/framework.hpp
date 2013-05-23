// Public Domain
#pragma once

#include "framework/plugin_api.hpp"

#include <memory>
#include <thread>
#include <vector>

class Framework
{
  public:
    Framework();
    ~Framework();  
    void Loop();
    
  private:   
    std::unique_ptr<Plugin> LoadPlugin(std::string name);
    std::vector<void*> handles; 
    std::vector<std::unique_ptr<Plugin>> plugins;
    std::vector<std::thread> threads;
};
