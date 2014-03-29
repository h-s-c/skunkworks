// Public Domain
#include <sstream>

#include <zeug/window.hpp>
#include <zeug/stringhash.hpp>
#include <msgpack.hpp>
#include <zmq.hpp>

#include <GLES2/gl2.h>

#include "plugins/common/entity.hpp"
#include "plugins/graphics/render.hpp"
#include "plugins/graphics/sprite.hpp"

std::uint32_t TextureManager::GetEmptySlot()
{
    slots++;
    return slots;
}

Render::Render(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber) : base_window(base_window), zmq_game_subscriber(zmq_game_subscriber)
{
    this->texturemanager = std::make_shared<TextureManager>();
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
}

void Render::operator()(double deltatime)
{
    zmq::message_t zmq_message;
    zmq_game_subscriber->recv(&zmq_message, ZMQ_NOBLOCK);
    if(zeug::stringhash("Sprite") == zeug::stringhash(zmq_message.data()))
    {
        zmq_message.rebuild();
        zmq_game_subscriber->recv(&zmq_message, 0);
        if (zeug::stringhash("Create") == zeug::stringhash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(zeug::stringhash("Finish") != zeug::stringhash(zmq_message.data()))
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
        else if(zeug::stringhash("Update") == zeug::stringhash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(zeug::stringhash("Finish") != zeug::stringhash(zmq_message.data()))
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
        glClear(GL_COLOR_BUFFER_BIT);
        this->akkumulator = 0.0f;
        for (auto& sprite : this->sprites)
        {
            sprite();
        }
    }
}
