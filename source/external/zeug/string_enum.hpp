#pragma once

/*
 *  Created on: Mar 20, 2012
 *      Author: Nicholas Folse
 *
 *  Copyright 2012 Nicholas Folse  
 *  License: Public Domain
 */

#include <string>
#include <vector>
#include <utility>

 #include <zeug/detail/stdfix.hpp>

namespace zeug
{

template <typename EnumMgr, typename Enum>
struct string_enum {
    typedef std::pair<Enum, std::string> pair_t;
    typedef std::vector<pair_t> vec_t;
    typedef typename vec_t::const_iterator vec_t_iter;
    static std::string toString(const Enum en);
    static Enum toEnum(const std::string &en);
};

template <typename EnumMgr, typename Enum>
std::string string_enum<EnumMgr, Enum>::toString(const Enum en) {
    for (vec_t_iter it = EnumMgr::en2str_vec.begin(); it < EnumMgr::en2str_vec.end(); it++) {
        if (en == it->first)
            return it->second;
    }
    return "";
}

template <typename EnumMgr, typename Enum>
Enum string_enum<EnumMgr, Enum>::toEnum(const std::string &en) {
    for (vec_t_iter it = EnumMgr::en2str_vec.begin(); it < EnumMgr::en2str_vec.end(); it++) {
        if (en == it->second)
            return it->first;
    }
    return (Enum)0;
}

} /* namespace base */
