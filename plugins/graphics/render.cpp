// Public Domain

#include "plugins/graphics/render.hpp"
#include "plugins/graphics/sprite.hpp"


Render::Render()
{
    this->sprites.push_back(Sprite("./test.png"));
}

void Render::Draw(double deltatime)
{
    for (auto& sprite : this->sprites)
    {
        sprite.Draw(deltatime);
    }
}
