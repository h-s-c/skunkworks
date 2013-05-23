// Public Domain
#pragma once

#include "framework/plugin_api.hpp"

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

class InputPlugin : public Plugin
{
  public:
    InputPlugin();
    virtual ~InputPlugin();
    virtual void Loop() final;  
    virtual void SetParameters(PluginParams* params) final { std::runtime_error e("Not Supported"); throw e; };
    virtual PluginParams* GetParameters() final { return &this->params; };
    
  private: 
    PluginParams params;
    void * input_socket;
    
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
};


