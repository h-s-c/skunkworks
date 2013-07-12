// Public Domain

#include "plugins/graphics/render.hpp"
#include "plugins/graphics/sprite.hpp"

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

std::uint32_t TextureManager::GetEmptySlot()
{
    slots++;
    return slots;
}

Render::Render()
{
    this->texturemanager = std::make_shared<TextureManager>();
    Sprite sprite1 = {this->texturemanager, "./../../assets/players/darksaber/sprite"};
    sprite1.SetPosition(std::make_pair(-100, -100));
    sprite1.SetScale(0.2f);
    sprite1.SetState(WalkRight);
    this->sprites.push_back(std::move(sprite1));
    Sprite sprite2 = {this->texturemanager, "./../../assets/players/darksaber/sprite"};
    sprite2.SetPosition(std::make_pair(100, 100));
    sprite2.SetScale(0.2f);
    sprite2.SetState(WalkLeft);
    this->sprites.push_back(std::move(sprite2));
    Sprite sprite3 = {this->texturemanager, "./../../assets/players/darksaber/sprite"};
    sprite3.SetPosition(std::make_pair(0, 0));
    sprite3.SetScale(0.2f);
    sprite3.SetState(IdleLeft);
    this->sprites.push_back(std::move(sprite3));
    /* hack */
    gl.Clear().ColorBuffer();
    gl.DrawElements(oglplus::PrimitiveType::Triangles, 6, (GLushort*)0);
}

void Render::Draw(double deltatime)
{
    this->akkumulator += deltatime;
    
    /* 60 frames / second due to animations */
    if( this->akkumulator >= 2000)
    {
        
        this->akkumulator = 0.0f;
        gl.Clear().ColorBuffer();
    
        for (auto& sprite : this->sprites)
        {
            sprite.Update();
            gl.DrawElements(oglplus::PrimitiveType::Triangles, 6, (GLushort*)0);
        }
    }
}
