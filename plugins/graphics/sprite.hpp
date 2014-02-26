#pragma once

#include <cfloat>
#include <cstdint>
#include <string>
#include <memory>
#include <utility>

#include <zeug/json.hpp>
#include <zeug/platform.hpp>
#include <zeug/stringenum.hpp>
#include <zeug/window.hpp>
#include <zeug/opengl/buffer.hpp>
#include <zeug/opengl/program.hpp>
#include <zeug/opengl/shader.hpp>
#include <zeug/opengl/texture.hpp>

class TextureManager;

enum class SpriteState
{
    IdleRight,
    IdleLeft,
    WalkRight,
    WalkLeft
};

class StateStringEnum : public zeug::stringenum<StateStringEnum, SpriteState>
{
    StateStringEnum();
public:
    static const vec_t en2str_vec;
};

class Sprite
{
  public:
    /* Construct sprite from json desc*/
    Sprite(const std::shared_ptr<zeug::window> &base_window, const std::shared_ptr<TextureManager> &texturemanager, std::string sprite_path, std::int32_t id);
    
    void SetPosition(std::pair<std::int32_t, std::int32_t> position);
    void SetScale(float scale);
    void SetState(SpriteState sprite_state);
    std::int32_t GetId() { return this->id; };
    
    void operator()();
    
#if defined(COMPILER_MSVC)
	Sprite(Sprite && other)
	{
		*this = other;
		other.~Sprite();
	}
	Sprite& operator=(Sprite && other)
	{
		if (this != &other)
		{
			this->~Sprite();
			*this = other;
			other.~Sprite();
		}
		return *this;
	}
#endif

  private:
  
    std::shared_ptr<zeug::window> base_window;
  
    /* Id of the entity this sprite belongs to. */
    std::int32_t id;
  
    /* Position x,y of in pixels. */
    std::pair<std::int32_t, std::int32_t> position;
    
    /* Scale (1.0f = original; 0.5 half;) */
    float scale;
    
    /* json object */
    std::vector<zeug::json::Object> json_objects;
    zeug::json::Object current_json_object;
    
    /* Texture slots */
    std::vector<std::unique_ptr<zeug::opengl::texture>> textures;
    std::vector<std::uint32_t> texture_slots;
    std::uint32_t current_texture_slot;
    
    /* Sprite state */
    SpriteState sprite_state;

    std::uint32_t frame_number;

    std::unique_ptr<zeug::opengl::program> shaderprog;

    std::unique_ptr<zeug::opengl::buffer> indices;
    std::unique_ptr<zeug::opengl::buffer> vertices;
    std::unique_ptr<zeug::opengl::buffer> texcoords;
};
