// Public Domain

#include "plugins/graphics/render.hpp"
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

Render::Render()
{
    this->texturemanager = std::make_shared<TextureManager>();
    gl.Clear().ColorBuffer();
}

void Render::Update(zmq::socket_t& zmq_game_subscriber)
{
    /*std::int64_t more;
    size_t more_size = sizeof (more);
    zmq_game_subscriber.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    if (more) ...*/
    zmq::message_t zmq_message;
    if (zmq_game_subscriber.recv(&zmq_message, ZMQ_NOBLOCK)) 
    {
        if (base::StringHash("Graphics") == base::StringHash(zmq_message.data()))
        {
            zmq_game_subscriber.recv(&zmq_message, 0);
            if (base::StringHash("Sprite") == base::StringHash(zmq_message.data()))
            {
                zmq_game_subscriber.recv(&zmq_message, 0);
                if (base::StringHash("Create") == base::StringHash(zmq_message.data()))
                {
                    std::int32_t id = -1;
                    std::string path = "";
                    std::int32_t position_x = -1;
                    std::int32_t position_y = -1;
                    float scale = 1.0f;
                    SpriteState state = SpriteState::IdleRight;
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("Id") == base::StringHash(zmq_message.data()))
                    {
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        id = *(static_cast<int*>(zmq_message.data()));
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("Path") == base::StringHash(zmq_message.data()))
                    {
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        std::for_each((char*)zmq_message.data(), (char*)zmq_message.data() + zmq_message.size(), [&path](char c){ path.push_back(c);});
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("PositionX") == base::StringHash(zmq_message.data()))
                    {
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        position_x = *(static_cast<int*>(zmq_message.data()));
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("PositionY") == base::StringHash(zmq_message.data()))
                    {
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        position_y = *(static_cast<int*>(zmq_message.data()));
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("Scale") == base::StringHash(zmq_message.data()))
                    {
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        
                        msgpack::unpacked unpacked;
                        msgpack::unpack(&unpacked, reinterpret_cast<char*>(zmq_message.data()), zmq_message.size());
                        msgpack::object obj = unpacked.get();
                        
                        obj.convert(&scale);
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("State") == base::StringHash(zmq_message.data()))
                    {
                        std::string str = "";
                        zmq_game_subscriber.recv(&zmq_message, 0);
                        std::for_each((char*)zmq_message.data(), (char*)zmq_message.data() + zmq_message.size(), [&str](char c){ str.push_back(c);});
                        state = StateStringEnum::toEnum(str);
                    }
                    zmq_game_subscriber.recv(&zmq_message, 0);
                    if (base::StringHash("Finish") == base::StringHash(zmq_message.data()))
                    {
                        Sprite sprite = {this->texturemanager, path};
                        sprite.SetPosition(std::make_pair(position_x,position_y));
                        sprite.SetScale(scale);
                        sprite.SetState(state);
                        this->sprites.push_back(std::move(sprite));
                    }
                }
            }
        }
    }
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
