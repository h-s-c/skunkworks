// Public Domain
#pragma once

#include "framework/plugin_api.hpp"

#include <atomic>
#include <string>
#include <thread>

#include <SDL2/SDL.h>

class GraphicsPlugin : public framework::Plugin
{
  public:
    GraphicsPlugin() : thread(), stop(false) {};
    virtual ~GraphicsPlugin() {};
    virtual void Start(void* parameters) final;  
    virtual void Stop() final;  
    
  private: 
    SDL_Window* window;
    SDL_GLContext context;
        
    void Run();
    
    std::thread thread;
    std::atomic<bool> stop;
};


