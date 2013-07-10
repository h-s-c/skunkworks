#pragma once
#include "base/parser/json.hpp"

#include <cfloat>
#include <cstdint>
#include <string>
#include <utility>

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

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
    Sprite(std::string sprite_path);
    
    void SetPosition(std::pair<std::int32_t, std::int32_t> position);
    void SetScale(float scale);
    void SetState(SpriteState sprite_state);
    void Draw(double deltatime);

  private:
    void Animate();
    float akkumulator;
  
    /* Position x,y of in pixels. */
    std::pair<std::int32_t, std::int32_t> position;
    bool position_changed;
    
    /* Scale (1.0f = original; 0.5 half;) */
    float scale;
    bool scale_changed;
    
    /* json object */
    std::vector<base::json::Object> json_objects;
    base::json::Object current_json_object;
    
    /* Sprite sheet */
    std::vector<oglplus::Texture> sprite_sheets;
    
    /* Sprite state */
    SpriteState sprite_state;
    
    std::int32_t frame_number;
    
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
    
    /* Uniforms */
    oglplus::LazyUniform<oglplus::Mat4f> projection_matrix;
};
