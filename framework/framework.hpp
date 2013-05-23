// Public Domain
#pragma once

#include "framework/plugin_api.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <stdexcept>

#include <SDL2/SDL.h>

class Framework
{
  public:
    Framework();
    ~Framework();  
    void Run();
    
  private: 
    void InitGL();
    SDL_GLContext context;
    SDL_Window* window; 

    template <typename F>
    auto SDL_CheckError(F f) -> F
    {
        if (!f) 
        {
            std::runtime_error e(SDL_GetError());
            SDL_Quit();
            throw e;
        }
        return f;
    }  
  
    void LoadPlugin(std::string name);
    struct Plugin
    {
        std::string name;
        void* handle;
        std::unique_ptr<framework::Plugin> pointer;
    };
    std::vector<Plugin> plugins; 
    
    bool done;
};
