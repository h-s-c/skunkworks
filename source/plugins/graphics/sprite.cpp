#include "plugins/graphics/sprite.hpp"
#include "plugins/graphics/render.hpp"

#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <memory>
#include <utility>

#include <zeug/memory_map.hpp>
#include <zeug/platform.hpp>
#include <zeug/window.hpp>
#include <zeug/zipreader.hpp>
#include <zeug/opengl/buffer.hpp>
#include <zeug/opengl/program.hpp>
#include <zeug/opengl/shader.hpp>
#include <zeug/opengl/texture.hpp>

#include <GLES2/gl2.h>

#include "external/json11/json11.hpp"

const Statestring_enum::vec_t Statestring_enum::en2str_vec = 
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
    // load zip/apk archive
    auto pakpath = [] () 
    {
        if (zeug::this_app::apkpath() == "Unknown")
        {
            return zeug::this_app::name() + ".pak";
        }
        else
        {
            return zeug::this_app::apkpath();
        }
    };

    zeug::zipreader pakfile(pakpath());
    // parse json descs 
    {
        std::string error;
        auto json_object = json11::Json::parse(pakfile.text_file(sprite_path + "/" + "idle.json"), error);
        if (!error.empty()) 
        {
            std::runtime_error(error.c_str());
        }
        this->json_objects.push_back(json_object);
    }
    {
        std::string error;
        auto json_object = json11::Json::parse(pakfile.text_file(sprite_path + "/" + "walk.json"), error);
        if (!error.empty()) 
        {
            std::runtime_error(error.c_str());
        }
        this->json_objects.push_back(json_object);
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
    this->texcoord_attrib= glGetAttribLocation(shaderprog->native_handle(), "TexCoord");
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
        auto name = json11::Json(this->json_objects.at(i))["meta"]["image"].string_value();

        // Texturepacker creates a nice unique id for us
        auto uid = json11::Json(this->json_objects.at(i))["meta"]["smartupdate"].string_value();
        uid.erase (1,26);

        auto w =json11::Json(this->json_objects.at(i))["meta"]["size"]["w"].int_value();
        auto h = json11::Json(this->json_objects.at(i))["meta"]["size"]["h"].int_value();
        auto texture = std::make_unique<zeug::opengl::texture>(uid, pakfile.file(sprite_path + "/" + name), std::make_pair(w, h));
        texture_slots.push_back(texture->native_slot());
        textures.push_back(std::move(texture));
    }
}

void Sprite::SetPosition(std::pair<std::int32_t, std::int32_t> position)
{
    this->position = position;
}

void Sprite::SetScale(float scale)
{
    this->scale = scale;
}

void Sprite::SetState(SpriteState sprite_state)
{
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

        if(this->frame_number >= json11::Json(this->current_json_object)["frames"].array_items().size())
        {
            // repeat
            this->frame_number = 0;
        }

        auto sprite_sheet_width = json11::Json(this->current_json_object)["meta"]["size"]["w"].int_value();
        auto sprite_sheet_height = json11::Json(this->current_json_object)["meta"]["size"]["h"].int_value();
        auto frame_object = json11::Json(this->current_json_object)["frames"].array_items().at(this->frame_number);
        auto frame_width = json11::Json(frame_object)["frame"]["w"].int_value();
        auto frame_height = json11::Json(frame_object)["frame"]["h"].int_value();
        auto frame_x =  json11::Json(frame_object)["frame"]["x"].int_value();
        auto frame_y =  json11::Json(frame_object)["frame"]["y"].int_value();

        // determine vertex positions
        std::vector<GLfloat> vertices = {
            float(((frame_width/2)*this->scale)+this->position.first),      float(((frame_height/2)*this->scale)+this->position.second),
            float(-(((frame_width/2)*this->scale)-this->position.first)),   float(((frame_height/2)*this->scale)+this->position.second),
            float(-(((frame_width/2)*this->scale)-this->position.first)),   float(-(((frame_height/2)*this->scale)-this->position.second)),
            float(((frame_width/2)*this->scale)+this->position.first),      float(-(((frame_height/2)*this->scale)-this->position.second)),
        };
        glEnableVertexAttribArray(this->position_attrib);
        glVertexAttribPointer(this->position_attrib, 2, GL_FLOAT, GL_TRUE, 0, &vertices[0]);

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
        glEnableVertexAttribArray(this->texcoord_attrib);
        glVertexAttribPointer(this->texcoord_attrib, 2, GL_FLOAT, GL_TRUE, 0, &texcoords[0]);

        std::vector<GLushort> elements = {
            0, 1, 2,
            2, 3, 0,
        };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &elements[0]);

        glDisableVertexAttribArray(this->position_attrib);
        glDisableVertexAttribArray(this->texcoord_attrib);

        this->akkumulator += deltatime;
        
        // 60 frames / second
        if( this->akkumulator >= 2000)
        {
            this->akkumulator = 0.0f;
            this->frame_number++;
        }
    }
}
