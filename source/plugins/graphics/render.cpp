// Public Domain
#include <sstream>

#include <zeug/window.hpp>
#include <zeug/string_hash.hpp>
#include <msgpack.hpp>
#include <zmq.hpp>

#include <GLES2/gl2.h>

#include "plugins/common/entity.hpp"
#include "plugins/graphics/render.hpp"
#include "plugins/graphics/sprite.hpp"

Render::Render(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<zmq::socket_t> &zmq_game_subscriber) : base_window(base_window), zmq_game_subscriber(zmq_game_subscriber)
{
    auto ogl_info = std::string("-----OGL-----\n") + 
        "Vendor: " + reinterpret_cast<const char*>(glGetString(GL_VENDOR)) + "\n" +
        "Renderer: " + reinterpret_cast<const char*>(glGetString(GL_RENDERER)) + "\n" +
        "Version: " + reinterpret_cast<const char*>(glGetString(GL_VERSION)) + "\n" +
        "Extensions: " + reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    std::cout << ogl_info << std::endl;
}

void Render::operator()(double deltatime)
{
    // Get entity updates
    zmq::message_t zmq_message;
    zmq_game_subscriber->recv(&zmq_message, ZMQ_NOBLOCK);
    if(zeug::string_hash("Sprite") == zeug::string_hash(zmq_message.data()))
    {
        zmq_message.rebuild();
        zmq_game_subscriber->recv(&zmq_message, 0);
        if (zeug::string_hash("Create") == zeug::string_hash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(zeug::string_hash("Finish") != zeug::string_hash(zmq_message.data()))
            {
                Entity entity{0, 0, 0, 0.0f, "", ""};
                msgpack::unpacked unpacked;
                msgpack::unpack(&unpacked, reinterpret_cast<char*>(zmq_message.data()), zmq_message.size());
                msgpack::object obj = unpacked.get();
                obj.convert(&entity);
                
                Sprite sprite = {this->base_window, entity.json_desc, entity.id};
                sprite.SetPosition(std::make_pair(entity.position_x, entity.position_y));
                sprite.SetScale(entity.scale);
                sprite.SetState(Statestring_enum::toEnum(entity.state));
                this->sprites.push_back(std::move(sprite));
                zmq_message.rebuild();
                zmq_game_subscriber->recv(&zmq_message, 0);
            }
        }
        else if(zeug::string_hash("Update") == zeug::string_hash(zmq_message.data()))
        {
            zmq_message.rebuild();
            zmq_game_subscriber->recv(&zmq_message, 0);
            while(zeug::string_hash("Finish") != zeug::string_hash(zmq_message.data()))
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
                        sprite.SetState(Statestring_enum::toEnum(entity.state));
                        break;
                    }
                }
                zmq_message.rebuild();
                zmq_game_subscriber->recv(&zmq_message, 0);
            }
        }
    }
    
    // Render sprites
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& sprite : this->sprites)
    {
        sprite(deltatime);
    }
    glBlendFunc (GL_ZERO, GL_ZERO);
    glDisable (GL_BLEND);
}
