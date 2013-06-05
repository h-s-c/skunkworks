// Public Domain

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

class Render
{
  public:
    Render();
    void Draw(double deltatime);
    
  private:
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;
    oglplus::VertexArray triangle;
    oglplus::Buffer verts;
};
