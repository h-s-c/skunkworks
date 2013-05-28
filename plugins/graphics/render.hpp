// Public Domain

#define GLCOREARB_PROTOTYPES
#include <GL/glcorearb.h>
#include <oglplus/all.hpp>

class Render
{
  public:
    Render();
    void Draw();
    
  private:
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;
    oglplus::VertexArray triangle;
    oglplus::Buffer verts;
};
