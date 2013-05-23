// Public Domain

#define GLEW_MX
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <oglplus/all.hpp>

class Render
{
  public:
    Render();
    void Display();
    
  private:
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;
    oglplus::VertexArray triangle;
    oglplus::Buffer verts;
};
