// Public Domain
#pragma once

#include "plugins/graphics/sprite.hpp"

#include <memory>
#include <vector>

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

class TextureManager
{
  public:
    TextureManager() : slots(-1) {};
    std::uint32_t GetEmptySlot();
    
  private:
    std::uint32_t slots;
};

class Render
{
  public:
    Render();
    void Draw(double deltatime);
    
  private:
    oglplus::Context gl;
    float akkumulator;
    std::vector<Sprite> sprites;
    std::shared_ptr<TextureManager> texturemanager;
};
