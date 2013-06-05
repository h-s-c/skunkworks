// Public Domain

#include "plugins/graphics/render.hpp"

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>

Render::Render()
{
    using namespace oglplus;

    // Set the vertex shader source
    vs.Source(" \
        #version 140\n \
        in vec3 Position; \
        void main(void) \
        { \
            gl_Position = vec4(Position, 1.0); \
        } \
    ");
    // compile it
    vs.Compile();

    // set the fragment shader source
    fs.Source(" \
        #version 140\n \
        out vec4 fragColor; \
        void main(void) \
        { \
            fragColor = vec4(1.0, 0.0, 0.0, 1.0); \
        } \
    ");
    // compile it
    fs.Compile();

    // attach the shaders to the program
    prog.AttachShader(vs);
    prog.AttachShader(fs);
    // link and use it
    prog.Link();
    prog.Use();

    // bind the VAO for the triangle
    triangle.Bind();

    GLfloat triangle_verts[9] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    // bind the VBO for the triangle vertices
    verts.Bind(Buffer::Target::Array);
    // upload the data
    Buffer::Data(
        Buffer::Target::Array,
        9,
        triangle_verts
    );
    // setup the vertex attribs array for the vertices
    VertexAttribArray vert_attr(prog, "Position");
    vert_attr.Setup(3, DataType::Float);
    vert_attr.Enable();

    gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl.ClearDepth(1.0f);
}

void Render::Draw(double deltatime)
{
    using namespace oglplus;

    gl.Clear().ColorBuffer().DepthBuffer();

    gl.DrawArrays(PrimitiveType::Triangles, 0, 3);
}
