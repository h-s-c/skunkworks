#pragma once
#include "base/system/window.hpp"
#include "base/string/stringenum.hpp"
#include "base/parser/json.hpp"

#include <cfloat>
#include <cstdint>
#include <string>
#include <memory>
#include <utility>

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>
#include <oglplus/bound/texture.hpp>

class TextureManager;

enum class SpriteState
{
    IdleRight,
    IdleLeft,
    WalkRight,
    WalkLeft
};

class StateStringEnum : public base::StringEnum<StateStringEnum, SpriteState>
{
    StateStringEnum();
public:
    static const vec_t en2str_vec;
};

class Sprite
{
  public:
    /* Construct sprite from json desc*/
    Sprite(const std::shared_ptr<base::Window> &base_window, const std::shared_ptr<TextureManager> &texturemanager, std::string sprite_path, std::int32_t id);
    
    void SetPosition(std::pair<std::int32_t, std::int32_t> position);
    void SetScale(float scale);
    void SetState(SpriteState sprite_state);
    std::int32_t GetId() { return this->id; };
    
    void operator()();

  private:
    std::shared_ptr<base::Window> base_window;
  
    /* Id of the entity this sprite belongs to. */
    std::int32_t id;
  
    /* Position x,y of in pixels. */
    std::pair<std::int32_t, std::int32_t> position;
    
    /* Scale (1.0f = original; 0.5 half;) */
    float scale;
    
    /* json object */
    std::vector<base::json::Object> json_objects;
    base::json::Object current_json_object;
    
    /* Texture slots */
    std::vector<oglplus::Texture> textures;
    std::vector<std::uint32_t> texture_slots;
    std::uint32_t current_texture_slot;
    
    /* Sprite state */
    SpriteState sprite_state;

    std::uint32_t frame_number;
    
    /* Wrapper around the current OpenGL context */
    oglplus::Context gl;

    /* Vertex and fragment shader */
    oglplus::Shader vs, fs;

    /* Program */
    oglplus::Program prog;

    /* A vertex array object for the rendered rectangle */
    oglplus::VertexArray rectangle;

    /* VBOs for the rectangle's vertices and colors */
    oglplus::Buffer verts;
    oglplus::Buffer indices;
    oglplus::Buffer texcoords;
};
