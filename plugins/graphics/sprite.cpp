#include "plugins/graphics/sprite.hpp"
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
#include <oglplus/images/load.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/opt/resources.hpp>

Sprite::Sprite(std::string sprite_path) : 
    position_changed(false), scale_changed(false),
    frame_number(0),
    vs(oglplus::ShaderType::Vertex), 
    fs(oglplus::ShaderType::Fragment), 
    projection_matrix(prog, "ProjectionMatrix")
{
    /* parse json descs */
    
    {
        std::ifstream json_file  (sprite_path+"/idle.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        base::json::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }
    
    {
        std::ifstream json_file  (sprite_path+"/walk.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        base::json::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }
    
    // Set the vertex shader source
    vs.Source(
        "#version 120\n"
        "uniform mat4 ProjectionMatrix;"
        "uniform vec2 Transformation;"
        "attribute vec2 Position;"
        "attribute vec2 TexCoord;"
        "varying vec2 vertTexCoord;"
        "void main(void)"
        "{"
        "   vertTexCoord = TexCoord;"
        "   gl_Position =  ProjectionMatrix *  vec4( Position*Transformation, 0.0, 1.0 );"
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
    
    /* bind the VAO for the rectangle */
    this->rectangle.Bind();
    
    /* initial vertex positions */
    GLfloat rectangle_verts[] = {
        1.0f,  1.0f,
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f,  -1.0f,
    };
    /* bind the VBO for the rectangle vertices */
    this->verts.Bind(oglplus::Buffer::Target::Array);
    /* upload the data */
    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, 8, rectangle_verts);
    
    // enable vertex attribs array for the vertices
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
    
    // setup the textures
    for( auto i=0; i < this->json_objects.size(); i++)
    {
        oglplus::Texture::Active(i);
        oglplus::Texture texture;
        auto bound_tex = oglplus::Bind(texture, oglplus::Texture::Target::_2D);
        sprite_sheets.push_back(std::move(texture));
        
        std::ifstream image_stream(sprite_path + "/" + this->json_objects.at(i).get<base::json::Object>("meta").get<base::json::String>("image"));
        
        bound_tex.Image2D(oglplus::images::PNG(image_stream));
        bound_tex.GenerateMipmap();
        bound_tex.MinFilter(oglplus::TextureMinFilter::LinearMipmapLinear);
        bound_tex.MagFilter(oglplus::TextureMagFilter::Linear);
        bound_tex.WrapS(oglplus::TextureWrap::ClampToEdge);
        bound_tex.WrapT(oglplus::TextureWrap::ClampToEdge);
    }
    
    /* set the projection matrix */
    projection_matrix.Set(
        oglplus::CamMatrixf::OrthoX(
            640.0f,
            4.0f/3.0f,
            1.0f, 0.0f)
    );
    
    gl.ClearDepth(1.0f);
}

void Sprite::SetPosition(std::pair<std::int32_t, std::int32_t> position)
{
    /* dirty flag */
    this->position_changed = true;
    /* set position values */
    this->position = position;
}

void Sprite::SetScale(float scale)
{
    /* dirty flag */
    this->scale_changed = true;
    /* set scale values */
    if(scale >= 0.1f)
    {
        this->scale = scale;
    }
}

void Sprite::SetState(SpriteState sprite_state)
{
    /* set state values */
    this->sprite_state = sprite_state;
    frame_number = 0;
    
    if( this->sprite_state == IdleRight || this->sprite_state == IdleLeft)
    {
        (prog/"TexUnit") = 0;
        this->current_json_object = this->json_objects.at(0);
    }
    else if( this->sprite_state == WalkRight || this->sprite_state == WalkLeft)
    {
        (prog/"TexUnit") = 1;
        this->current_json_object = this->json_objects.at(1);
    }
}

void Sprite::Draw(double deltatime)
{    
    /* animate sprite */
    akkumulator += deltatime;
    
    /* 60 frames / second */
    if( akkumulator >= 2000)
    {
        akkumulator = 0.0f;
    
       Animate();
       frame_number++;
    }
    
    gl.Clear().ColorBuffer().DepthBuffer();

    gl.DrawElements(oglplus::PrimitiveType::Triangles, 6, (GLushort*)0);
}

void Sprite::Animate()
{
    /* animate sprite */
    if(frame_number == this->current_json_object.get<base::json::Array>("frames").size())
    {
        /* repeat */
        frame_number = 0;
    }
    auto sprite_sheet_width = this->current_json_object.get<base::json::Object>("meta").get<base::json::Object>("size").get<base::json::Number>("w");
    auto sprite_sheet_height = this->current_json_object.get<base::json::Object>("meta").get<base::json::Object>("size").get<base::json::Number>("h");
    auto frame = this->current_json_object.get<base::json::Array>("frames").get<base::json::Object>(frame_number).get<base::json::Object>("frame");
    auto frame_width = frame.get<base::json::Number>("w");
    auto frame_height = frame.get<base::json::Number>("h");
    auto frame_x = frame.get<base::json::Number>("x");
    auto frame_y = frame.get<base::json::Number>("y");
    
    if(this->scale_changed || this->position_changed)
    {        
        /* set the transformation vector */
        oglplus::Uniform<oglplus::Vec2f>(this->prog, "Transformation").Set(
            float(((frame_width/2)*this->scale)+this->position.first),  
            float(((frame_height/2)*this->scale)+this->position.second)
        );
        
        /* reset dirty flag */
        this->scale_changed = false;
        this->position_changed = false;
    }
    
    std::vector<GLfloat> rectangle_texcoords;
    if( this->sprite_state == IdleRight || this->sprite_state == WalkRight)
    {
        /* determine texcoords */
        rectangle_texcoords = {
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
        };
    }
    else if( this->sprite_state == IdleLeft || this->sprite_state == WalkLeft)
    {
        /* determine texcoords */
        rectangle_texcoords = {
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
        };
    }
    
    // disable the vertex attribs array for the texcoords
    {
        oglplus::VertexAttribArray vert_attr(prog, "TexCoord");
        vert_attr.Setup<oglplus::Vec2f>().Disable();
    }
    
    /* bind the VBO for the rectangle texcoords */
    texcoords.Bind(oglplus::Buffer::Target::Array);
    /* upload the data */
    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, rectangle_texcoords);
    
    // enable the vertex attribs array for the texcoords
    {
        oglplus::VertexAttribArray vert_attr(prog, "TexCoord");
        vert_attr.Setup<oglplus::Vec2f>().Enable();
    }
}
