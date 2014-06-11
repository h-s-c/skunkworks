#include "plugins/graphics/sprite.hpp"
#include "plugins/graphics/render.hpp"

#include <cfloat>
#include <cstdint>
#include <fstream>
#include <string>
#include <memory>
#include <utility>

#include <zeug/memory_map.hpp>
#include <zeug/platform.hpp>
#include <zeug/window.hpp>
#include <zeug/opengl/buffer.hpp>
#include <zeug/opengl/program.hpp>
#include <zeug/opengl/shader.hpp>
#include <zeug/opengl/texture.hpp>

#include <GLES2/gl2.h>

#include "external/jsonxx.hpp"

const StateStringEnum::vec_t StateStringEnum::en2str_vec = 
{
        pair_t(SpriteState::None, "None"),
        pair_t(SpriteState::IdleRight, "IdleRight"),
        pair_t(SpriteState::IdleLeft, "IdleLeft"),
        pair_t(SpriteState::WalkRight, "WalkRight"),
        pair_t(SpriteState::WalkLeft, "WalkLeft"),
};

Sprite::Sprite(const std::shared_ptr<zeug::window> &base_window, std::string sprite_path, std::int32_t id) :
    base_window(base_window), 
    id(id),
    frame_number(0)
{
    // parse json descs 
    {
        std::ifstream json_file(sprite_path+"/idle.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        jsonxx::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }
    {
        std::ifstream json_file (sprite_path+"/walk.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        jsonxx::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }

    auto vertexshader_string = R"(
    #version 100
    #ifdef GL_FRAGMENT_PRECISION_HIGH
    varying highp vec2 vertTexCoord;
    #else
    varying mediump vec2 vertTexCoord;
    #endif
    
    uniform mat4 ProjectionMatrix;
    attribute vec2 Position;
    attribute vec2 TexCoord;
    
    void main(void)
    {
       vertTexCoord = TexCoord;
       gl_Position =  ProjectionMatrix *  vec4( Position, 0.0, 1.0 );
    }
    )"; 

    auto fragmentshader_string = R"(
    #version 100
    #ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
    #else
    precision mediump float;
    #endif
    
    uniform sampler2D TexUnit;
    varying vec2 vertTexCoord;
    void main(void)
    {
        gl_FragColor = texture2D( TexUnit, vertTexCoord );
    }
    )"; 

    this->shaderprog = std::make_unique<zeug::opengl::program>();
    this->shaderprog.get()->attach(std::make_unique<zeug::opengl::shader>(vertexshader_string, GL_VERTEX_SHADER));
    this->shaderprog.get()->attach(std::make_unique<zeug::opengl::shader>(fragmentshader_string, GL_FRAGMENT_SHADER));
    this->shaderprog.get()->link();

    this->position_attrib= glGetAttribLocation(shaderprog->native_handle(), "Position");
    glEnableVertexAttribArray(this->position_attrib);

    this->texcoord_attrib= glGetAttribLocation(shaderprog->native_handle(), "TexCoord");
    glEnableVertexAttribArray(this->texcoord_attrib);

    this->texunit_uniform = glGetUniformLocation(this->shaderprog.get()->native_handle(), "TexUnit");
    this->proj_uniform = glGetUniformLocation(shaderprog->native_handle(), "ProjectionMatrix");

    auto width = float(this->base_window->width());
    auto aspect = float(this->base_window->width() / this->base_window->height());
    auto near = 1.0f;
    auto far = -1.0f;

    auto right = width / 2.0f;
    auto left = -right;

    auto bottom = left / aspect;
    auto top = right / aspect;

    auto a = 2.0f / (right - left);
    auto b = 2.0f / (top - bottom);
    auto c = -2.0f / (far - near);

    auto tx = - (right + left)/(right - left);
    auto ty = - (top + bottom)/(top - bottom);
    auto tz = - (far + near)/(far - near);

    this->ortho_projection = 
    {{
        a,      0.0f,   0.0f,   0.0f,
        0.0f,   b,      0.0f,   0.0f,
        0.0f,   0.0f,   c,      0.0f,
        tx,     ty,     tz,     1.0f
    }};

    for( std::uint32_t i=0; i < this->json_objects.size(); i++)
    {        
        auto name = this->json_objects.at(i).get<jsonxx::Object>("meta").get<jsonxx::String>("image");

        // Texturepacker creates a nice unique id for us
        auto uid = this->json_objects.at(i).get<jsonxx::Object>("meta").get<jsonxx::String>("smartupdate"); 
        uid.erase (1,26);

        auto w = this->json_objects.at(i).get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("w");
        auto h = this->json_objects.at(i).get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("h");

        auto texture = std::make_unique<zeug::opengl::texture>(uid, sprite_path, name, std::make_pair(w, h));
        texture_slots.push_back(texture->native_slot());
        textures.push_back(std::move(texture));
    }
}

void Sprite::SetPosition(std::pair<std::int32_t, std::int32_t> position)
{
    /* set position values */
    this->position = position;
}

void Sprite::SetScale(float scale)
{
    /* set scale values */
    this->scale = scale;
}

void Sprite::SetState(SpriteState sprite_state)
{
    // set state values
    if(sprite_state != this->sprite_state)
    {
        this->sprite_state = sprite_state;
        frame_number = 0;
        
        if( this->sprite_state == SpriteState::IdleRight || this->sprite_state == SpriteState::IdleLeft)
        {
            this->current_json_object = this->json_objects.at(0);
            this->current_texture_slot = this->texture_slots.at(0);
        }
        else if( this->sprite_state == SpriteState::WalkRight || this->sprite_state == SpriteState::WalkLeft)
        {
            this->current_json_object = this->json_objects.at(1);
            this->current_texture_slot = this->texture_slots.at(1);
        }
    }
}

void Sprite::operator()(double deltatime)
{
    if(!this->ready)
    {
        this->ready = true;
        for(auto& texture : textures)
        {
            if(!texture->ready())
            {
                this->ready = false;
            }

        }
    }

    else if(this->sprite_state != SpriteState::None)
    {
        this->shaderprog.get()->use();

        glUniform1i(this->texunit_uniform, this->current_texture_slot);
        glUniformMatrix4fv(this->proj_uniform, 1, 0, &this->ortho_projection[0]);

        if(this->frame_number >= this->current_json_object.get<jsonxx::Array>("frames").size())
        {
            // repeat
            this->frame_number = 0;
        }

        auto sprite_sheet_width = this->current_json_object.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("w");
        auto sprite_sheet_height = this->current_json_object.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("h");
        auto frame = this->current_json_object.get<jsonxx::Array>("frames").get<jsonxx::Object>(this->frame_number).get<jsonxx::Object>("frame");
        auto frame_width = frame.get<jsonxx::Number>("w");
        auto frame_height = frame.get<jsonxx::Number>("h");
        auto frame_x = frame.get<jsonxx::Number>("x");
        auto frame_y = frame.get<jsonxx::Number>("y");

        // determine vertex positions
        std::vector<GLfloat> vertices = {
            float(((frame_width/2)*this->scale)+this->position.first),      float(((frame_height/2)*this->scale)+this->position.second),
            float(-(((frame_width/2)*this->scale)-this->position.first)),   float(((frame_height/2)*this->scale)+this->position.second),
            float(-(((frame_width/2)*this->scale)-this->position.first)),   float(-(((frame_height/2)*this->scale)-this->position.second)),
            float(((frame_width/2)*this->scale)+this->position.first),      float(-(((frame_height/2)*this->scale)-this->position.second)),
        };
        glVertexAttribPointer(this->position_attrib, 2, GL_FLOAT, GL_FALSE, 0, &vertices[0]);

        // determine texcoords
        std::vector<GLfloat> texcoords;
        if( this->sprite_state == SpriteState::IdleRight || this->sprite_state == SpriteState::WalkRight)
        {
            texcoords = {
                float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
                float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
                float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
                float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            };
        }
        else if( this->sprite_state == SpriteState::IdleLeft || this->sprite_state == SpriteState::WalkLeft)
        {
            texcoords = {
                float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
                float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
                float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
                float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            };
        }
        glVertexAttribPointer(this->texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, &texcoords[0]);

        std::vector<GLushort> elements = {
            0, 1, 2,
            2, 3, 0,
        };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &elements[0]);

        this->akkumulator += deltatime;
        
        // 60 frames / second
        if( this->akkumulator >= 2000)
        {
            this->akkumulator = 0.0f;
            this->frame_number++;
        }
    }
}
