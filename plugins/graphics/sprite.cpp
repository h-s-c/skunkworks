#include "plugins/graphics/sprite.hpp"

#include <oglplus/gl.hpp>
/* X11 sucks */
#undef Expose
#undef None
#include <oglplus/all.hpp>
#include <oglplus/images/load.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/opt/resources.hpp>

Sprite::Sprite(std::string image_path): vs(oglplus::ShaderType::Vertex), fs(oglplus::ShaderType::Fragment), projection_matrix(prog, "ProjectionMatrix")
{
    // Set the vertex shader source
    vs.Source(
        "#version 120\n"
        "uniform mat4 ProjectionMatrix;"
        "attribute vec2 Position;"
        "attribute vec2 TexCoord;"
        "varying vec2 vertTexCoord;"
        "void main(void)"
        "{"
        "   vertTexCoord = TexCoord;"
        "   gl_Position =  ProjectionMatrix * vec4( Position, 0.0, 1.0 );"
        "}"
    );
    // compile it
    vs.Compile();

    // set the fragment shader source
    fs.Source(
        "#version 120\n"
        "uniform sampler2D TexUnit;"
        "varying vec2 vertTexCoord;"
        "void main(void)"
        "{"
        "   gl_FragColor = texture2D( TexUnit, vertTexCoord );"
        "}"
    );
    fs.Compile();

    // attach the shaders to the program
    prog.AttachShader(vs);
    prog.AttachShader(fs);
    // link and use it
    prog.Link();
    prog.Use();

    // bind the VAO for the rectangle
    rectangle.Bind();
    
    this->position = {0, 0};
    this->scale = {480, 480};

    GLfloat rectangle_verts[] = {
         (scale.first/2)+position.first,  (scale.second/2)+position.second,
        -(scale.first/2)+position.first,  (scale.second/2)+position.second,
        -(scale.first/2)+position.first, -(scale.second/2)+position.second,
         (scale.first/2)+position.first, -(scale.second/2)+position.second,
    };
    // bind the VBO for the rectangle vertices
    verts.Bind(oglplus::Buffer::Target::Array);
    // upload the data
    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, 8, rectangle_verts);
    
    // setup the vertex attribs array for the vertices
    {
        oglplus::VertexAttribArray vert_attr(prog, "Position");
        vert_attr.Setup<oglplus::Vec2f>().Enable();
    }
    
    GLushort rectangle_elements[] = {
        0, 1, 2,
        2, 3, 0,
    };
    
    // bind the VBO for the indices
    indices.Bind(oglplus::Buffer::Target::ElementArray);
    // upload them
    oglplus::Buffer::Data(oglplus::Buffer::Target::ElementArray, rectangle_elements);
    
    GLfloat rectangle_texcoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    // bind the VBO for the rectangle vertices
    texcoords.Bind(oglplus::Buffer::Target::Array);
    // upload the data
    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, 8, rectangle_texcoords);
        
    // setup the vertex attribs array for the texcoords
    {
        oglplus::VertexAttribArray vert_attr(prog, "TexCoord");
        vert_attr.Setup<oglplus::Vec2f>().Enable();
    }

    // setup the texture
    {
        auto bound_tex = oglplus::Bind(sprite_sheet, oglplus::Texture::Target::_2D);
        
        std::ifstream image_stream(image_path);
        
        bound_tex.Image2D(oglplus::images::PNG(image_stream));
        bound_tex.GenerateMipmap();
        bound_tex.MinFilter(oglplus::TextureMinFilter::LinearMipmapLinear);
        bound_tex.MagFilter(oglplus::TextureMagFilter::Linear);
        bound_tex.WrapS(oglplus::TextureWrap::ClampToEdge);
        bound_tex.WrapT(oglplus::TextureWrap::ClampToEdge);
    }
    // set the uniform values
    (prog/"TexUnit") = 0;
    
    projection_matrix.Set(
        oglplus::CamMatrixf::OrthoX(
            640.0f,
            4.0f/3.0f,
            1.0f, 0.0f)
    );
    
    gl.ClearDepth(1.0f);
}

void Sprite::Draw(double deltatime)
{
    gl.Clear().ColorBuffer().DepthBuffer();

    gl.DrawElements(oglplus::PrimitiveType::Triangles, 6, (GLushort*)0);
        
    /*const float verts[] = {
        posX, posY,
        posX + spriteWidth, posY,
        posX + spriteWidth, posY + spriteHeight,
        posX, posY + spriteHeight
    };
    const float tw = float(spriteWidth) / texWidth;
    const float th = float(spriteHeight) / texHeight;
    const int numPerRow = texWidth / spriteWidth;
    const float tx = (frameIndex % numPerRow) * tw;
    const float ty = (frameIndex / numPerRow + 1) * th;
    const float texVerts[] = {
        tx, ty,
        tx + tw, ty,
        tx + tw, ty + th,
        tx, ty + th
    };

    // ... Bind the texture, enable the proper arrays

    glVertexPointer(2, GL_FLOAT, verts);
    glTextureVertexPointer(2, GL_FLOAT, texVerts);
    glDrawArrays(GL_TRI_STRIP, 0, 4);*/
}

