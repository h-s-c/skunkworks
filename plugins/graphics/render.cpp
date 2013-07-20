// Public Domain

#include "plugins/graphics/render.hpp"
#include "base/system/window.hpp"
#include "plugins/common/entity.hpp"
#include "plugins/graphics/sprite.hpp"
#include "base/string/stringhash.hpp"

#include <sstream>

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

#include <zmq.hpp>
#include <msgpack.hpp>

std::uint32_t TextureManager::GetEmptySlot()
{
    slots++;
    return slots;
}

Render::Render(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber) : base_window(base_window), zmq_game_subscriber(zmq_game_subscriber)
{
    this->texturemanager = std::make_shared<TextureManager>();
    gl.Clear().ColorBuffer();
}

void Render::operator()(double deltatime)
{
    zmq::message_t zmq_message;
    zmq_game_subscriber->recv(&zmq_message, ZMQ_NOBLOCK);
    if(base::StringHash("Sprite") == base::StringHash(zmq_message.data()))
    {
        zmq_message.rebuild();
        zmq_game_subscriber->recv(&zmq_message, 0);
        if (base::StringHash("Create") == base::StringHash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(base::StringHash("Finish") != base::StringHash(zmq_message.data()))
            {
                Entity entity{0, 0, 0, 0.0f, "", ""};
                msgpack::unpacked unpacked;
                msgpack::unpack(&unpacked, reinterpret_cast<char*>(zmq_message.data()), zmq_message.size());
                msgpack::object obj = unpacked.get();
                obj.convert(&entity);
                
                Sprite sprite = {this->base_window, this->texturemanager, entity.json_desc, entity.id};
                sprite.SetPosition(std::make_pair(entity.position_x, entity.position_y));
                sprite.SetScale(entity.scale);
                sprite.SetState(StateStringEnum::toEnum(entity.state));
                this->sprites.push_back(std::move(sprite));
                zmq_message.rebuild();
                zmq_game_subscriber->recv(&zmq_message, 0);
            }
        }
        else if(base::StringHash("Update") == base::StringHash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(base::StringHash("Finish") != base::StringHash(zmq_message.data()))
            {
                Entity entity{0, 0, 0, 0.0f, "", ""};
                msgpack::unpacked unpacked;
                msgpack::unpack(&unpacked, reinterpret_cast<char*>(zmq_message.data()), zmq_message.size());
                msgpack::object obj = unpacked.get();
                obj.convert(&entity);
                
                for (auto& sprite : this->sprites)
                {
                    //break;
                    if( sprite.GetId() == entity.id)
                    {
                        sprite.SetPosition(std::make_pair(entity.position_x, entity.position_y));
                        sprite.SetScale(entity.scale);
                        sprite.SetState(StateStringEnum::toEnum(entity.state));
                        break;
                    }
                }
                zmq_message.rebuild();
                zmq_game_subscriber->recv(&zmq_message, 0);
            }
        }
    }
    
    this->akkumulator += deltatime;
    
    /* 60 frames / second due to animations */
    if( this->akkumulator >= 2000)
    {
        
        this->akkumulator = 0.0f;
        gl.Clear().ColorBuffer();
        for (auto& sprite : this->sprites)
        {
            sprite();
        }
    }
}
