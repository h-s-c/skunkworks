// Public Domain
#pragma once

#include "framework/plugin_api.hpp"

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

class GraphicsPlugin : public Plugin
{
  public:
    GraphicsPlugin() : params() {};
    virtual ~GraphicsPlugin() {};
    virtual void Loop() final;  
    virtual void SetParameters(PluginParams* params) final { this->params = *params; };
    virtual PluginParams* GetParameters() final { std::runtime_error e("Not Supported"); throw e; };   
    
    /*virtual void SetContext(SDL_GLContext context) final { this->context = context; };
    virtual void SetWindow(SDL_Window* window) final { this->window = window; };
    virtual SDL_GLContext GetContext() final { std::runtime_error e("Not Supported"); throw e; };
    virtual SDL_Window* GetWindow() final { std::runtime_error e("Not Supported"); throw e; };*/
    
  private: 
    PluginParams params;
};


