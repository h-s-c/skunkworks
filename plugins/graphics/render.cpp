// Public Domain

#include "plugins/graphics/render.hpp"
#include "plugins/graphics/sprite.hpp"

Render::Render()
{
    Sprite sprite1 = {"./../../assets/players/darksaber/sprite"};
    sprite1.SetPosition(std::make_pair(0, 0));
    sprite1.SetScale(0.5f);
    sprite1.SetState(WalkLeft);
    this->sprites.push_back(std::move(sprite1));
}

void Render::Draw(double deltatime)
{
    for (auto& sprite : this->sprites)
    {
        sprite.Draw(deltatime);
    }
}
