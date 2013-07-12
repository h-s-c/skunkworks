#pragma once
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

enum SpriteState
{
    IdleRight = 0,
    IdleLeft,
    WalkRight,
    WalkLeft,
};


class Sprite
{
  public:
    /* Construct sprite from json desc*/
    Sprite(const std::shared_ptr<TextureManager> &texturemanager, std::string sprite_path);
    
    void SetPosition(std::pair<std::int32_t, std::int32_t> position);
    void SetScale(float scale);
    void SetState(SpriteState sprite_state);
    void Update();

  private:
  
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
