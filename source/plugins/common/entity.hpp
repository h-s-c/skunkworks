#pragma once
#include <cstdint>
#include <string>

#include <msgpack.hpp>

struct Entity
{
    std::int32_t id;
    std::int32_t position_x;
    std::int32_t position_y;
    float scale;
    std::string json_desc;
    std::string state;
    MSGPACK_DEFINE(id, position_x, position_y, scale, json_desc, state)
};
