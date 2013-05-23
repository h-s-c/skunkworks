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
    
    /*virtual void SetContext(SDL_GLContext context) final { std::runtime_error e("Not Supported"); throw e; };
    virtual void SetWindow(SDL_Window* window) final { std::runtime_error e("Not Supported"); throw e; };
    virtual SDL_GLContext GetContext() final { return this->context; };
    virtual SDL_Window* GetWindow() final { return this->window; };*/
    
  private: 
    PluginParams params;
    bool done;
    
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


