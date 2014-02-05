#include "plugins/graphics/sprite.hpp"
#include "plugins/graphics/render.hpp"
#include "plugins/graphics/opengl/program.hpp"
#include "plugins/graphics/opengl/shader.hpp"
#include "base/parser/json.hpp"

#include <cfloat>
#include <cstdint>
#include <fstream>
#include <string>
#include <memory>
#include <utility>

#include <platt/memory_map.hpp>
#include <platt/platform.hpp>
#include <platt/window.hpp>

#include <GLES2/gl2.h>

const StateStringEnum::vec_t StateStringEnum::en2str_vec = 
{
        pair_t(SpriteState::IdleRight, "IdleRight"),
        pair_t(SpriteState::IdleLeft, "IdleLeft"),
        pair_t(SpriteState::WalkRight, "WalkRight"),
        pair_t(SpriteState::WalkLeft, "WalkLeft"),
};

Sprite::Sprite(const std::shared_ptr<platt::window> &base_window, const std::shared_ptr<TextureManager> &texturemanager, std::string sprite_path, std::int32_t id) :
    base_window(base_window), 
    id(id),
    frame_number(0)
{
    /* parse json descs */
    
    {
        std::ifstream json_file(sprite_path+"/idle.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        base::json::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }
    
    {
        std::ifstream json_file (sprite_path+"/walk.json");
        std::stringstream json_buffer;
        json_buffer << json_file.rdbuf();
        json_file.close();

        base::json::Object json_object;
        json_object.parse(json_buffer);
        this->json_objects.push_back(std::move(json_object));
    }

    auto vertexshader_string = R"(
    #version 100
    #ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
    #else
    precision mediump float;
    #endif
    
    uniform mat4 ProjectionMatrix;
    attribute vec2 Position;
    attribute vec2 TexCoord;
    varying vec2 vertTexCoord;
    
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

    this->shaderprogram = std::make_unique<opengl::program>();
    this->shaderprogram.get()->attach(std::make_unique<opengl::shader>(vertexshader_string, GL_VERTEX_SHADER));
    this->shaderprogram.get()->attach(std::make_unique<opengl::shader>(fragmentshader_string, GL_FRAGMENT_SHADER));
    this->shaderprogram.get()->link();

    GLushort rectangle_elements[] = {
        0, 1, 2,
        2, 3, 0,
    };
    
    for( std::uint32_t i=0; i < this->json_objects.size(); i++)
    {        
        auto texture_slot = texturemanager->GetEmptySlot();

        platt::memory_map file(sprite_path + "/", this->json_objects.at(i).get<base::json::Object>("meta").get<base::json::String>("image"));
        auto texture = std::make_unique<opengl::texture>(std::move(file.memory));
        textures.push_back(std::move(texture));
        texture_slots.push_back(texture_slot);
    }

    glClearColor(0.0f, 0.5f, 1.0f, 1.0f); 
}

void Sprite::SetPosition(std::pair<std::int32_t, std::int32_t> position)
{
    /* set position values */
    this->position = position;
}

void Sprite::SetScale(float scale)
{
    /* set scale values */
    if(scale >= 0.1f)
    {
        this->scale = scale;
    }
    else
    {
        this->scale = 1.0f;
    }
}

void Sprite::SetState(SpriteState sprite_state)
{
    /* set state values */
    if(sprite_state != this->sprite_state)
    {
        this->sprite_state = sprite_state;
        frame_number = 0;
        
        if( this->sprite_state == SpriteState::IdleRight || this->sprite_state == SpriteState::IdleLeft)
        {
            this->current_json_object = this->json_objects.at(0);
            //this->current_texture_slot = this->texture_slots.at(0);
        }
        else if( this->sprite_state == SpriteState::WalkRight || this->sprite_state == SpriteState::WalkLeft)
        {
            this->current_json_object = this->json_objects.at(1);
            //this->current_texture_slot = this->texture_slots.at(1);
        }
    }
}

void Sprite::operator()()
{
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
      
    /* determine vertex positions */
    GLfloat rectangle_verts[] = {
        float(((frame_width/2)*this->scale)+this->position.first),   float(((frame_height/2)*this->scale)+this->position.second),
        float(-(((frame_width/2)*this->scale)-this->position.first)),   float(((frame_height/2)*this->scale)+this->position.second),
        float(-(((frame_width/2)*this->scale)-this->position.first)),   float(-(((frame_height/2)*this->scale)-this->position.second)),
        float(((frame_width/2)*this->scale)+this->position.first),   float(-(((frame_height/2)*this->scale)-this->position.second)),
    };

    std::vector<GLfloat> rectangle_texcoords;
    if( this->sprite_state == SpriteState::IdleRight || this->sprite_state == SpriteState::WalkRight)
    {
        /* determine texcoords */
        rectangle_texcoords = {
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
        };
    }
    else if( this->sprite_state == SpriteState::IdleLeft || this->sprite_state == SpriteState::WalkLeft)
    {
        /* determine texcoords */
        rectangle_texcoords = {
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*frame_y)),
            float((1.0f/sprite_sheet_width)*(frame_x+frame_width)),    float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
            float((1.0f/sprite_sheet_width)*frame_x),                  float(1.0f-((1.0f/sprite_sheet_height)*(frame_y+frame_height))),
        };
    }
    
    glClear(GL_COLOR_BUFFER_BIT);

    this->frame_number++;
}
