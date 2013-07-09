#pragma once
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
    IdleUp,
    IdleDown,
    MoveRight,
    MoveLeft,
    MoveUp,
    MoveDown,
};


class Sprite
{
  public:
    /* Construct sprite from image*/
    Sprite(std::string image_path);
    
    void SetPosition(std::pair<std::int64_t, std::int64_t> position){};
    void SetScale(std::int64_t scale){};
    void SetState(SpriteState sprite_state){};
    void Draw(double deltatime);

  private:
    /* Position x,y of in pixels. */
    std::pair<std::int64_t, std::int64_t> position;
    
    /* Scale in pixels. */
    std::pair<std::int64_t, std::int64_t> scale;
  
    /* Bitmap name. */
    std::string bitmap_name;
    
    /* Sprite sheet */
    oglplus::Texture sprite_sheet;
    
    /* Sprite state */
    SpriteState sprite_state;
    
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
