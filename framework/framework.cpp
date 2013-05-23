// Public Domain

#include "framework/framework.hpp"
#include "framework/plugin_api.hpp"
#include "base/platform.hpp"
#include "base/system/info.hpp"
#include "base/system/library.hpp"

#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
#include <utility>

#include <SDL2/SDL.h>
    
Framework::Framework()
{
    auto plugin_input = std::move(LoadPlugin("Input"));
    
    auto plugin_graphics = std::move(LoadPlugin("Graphics"));  
    plugin_graphics.get()->SetParameters(plugin_input.get()->GetParameters());
    
    this->plugins.push_back(std::move(plugin_input));
    this->plugins.push_back(std::move(plugin_graphics));
}

Framework::~Framework()
{
    for ( auto& plugin : this->plugins)
    {       
        plugin.release();
    }
    plugins.clear();
    
    for ( auto& handle : this->handles)
    {
        base::CloseLibrary(handle);
    }
}

void Framework::Loop()
{      
    for(std::uint32_t i=1; i<this->plugins.size(); ++i)
    {
        this->threads.push_back(std::thread([this, i](){ this->plugins[i].get()->Loop(); }));
    }
    
    plugins.front().get()->Loop();
    
    for ( auto& thread : this->threads)
    {       
        thread.join();
    }
}

std::unique_ptr<Plugin> Framework::LoadPlugin(std::string name)
{  
    auto handle = base::OpenLibrary(std::string("Plugin"+name), base::GetExecutableFilePath());
    auto funcs = reinterpret_cast<PluginFuncs*>(base::GetSymbol(handle, name ));
    
    if (!funcs)
    {
        std::runtime_error e(std::string("Could not load plugin ") + name);
        throw e;
    }
    
    handles.push_back(&handle);
    return std::move(funcs->InitPlugin());
}
