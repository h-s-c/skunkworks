#pragma once

#include <cfloat>
#include <cstdint>
#include <string>
#include <memory>
#include <utility>

#include <zeug/platform.hpp>
#include <zeug/string_enum.hpp>
#include <zeug/window.hpp>
#include <zeug/opengl/buffer.hpp>
#include <zeug/opengl/program.hpp>
#include <zeug/opengl/shader.hpp>
#include <zeug/opengl/texture.hpp>

#include "external/jsonxx.hpp"

#undef None
enum class SpriteState
{
    None,
    IdleRight,
    IdleLeft,
    WalkRight,
    WalkLeft
};

class Statestring_enum : public zeug::string_enum<Statestring_enum, SpriteState>
{
    Statestring_enum();
public:
    static const vec_t en2str_vec;
};

class Sprite
{
  public:
    /* Construct sprite from json desc*/
    Sprite(const std::shared_ptr<zeug::window> &base_window, std::string sprite_path, std::int32_t id);
    void SetPosition(std::pair<std::int32_t, std::int32_t> position);
    void SetScale(float scale);
    void SetState(SpriteState sprite_state);
    std::int32_t GetId() { return this->id; };
    
    void operator()(double deltatime);
    
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
    std::vector<jsonxx::Object> json_objects;
    jsonxx::Object current_json_object;
    
    /* Texture slots */
    std::vector<std::unique_ptr<zeug::opengl::texture>> textures;
    std::vector<std::uint32_t> texture_slots;
    std::uint32_t current_texture_slot;
    
    /* Sprite state */
    SpriteState sprite_state;

    std::uint32_t frame_number;

    std::unique_ptr<zeug::opengl::program> shaderprog;
    std::array<float, 16> ortho_projection;
    std::uint32_t texcoord_attrib;
    std::uint32_t position_attrib;
    std::uint32_t texunit_uniform;
    std::uint32_t proj_uniform;

    double akkumulator;
    double ready = false;
};
