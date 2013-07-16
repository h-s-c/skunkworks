#pragma once

#include <string>
#include <vector>
#include <utility>

namespace base
{

template <typename EnumMgr, typename Enum>
struct StringEnum {
    typedef std::pair<Enum, std::string> pair_t;
    typedef std::vector<pair_t> vec_t;
    typedef typename vec_t::const_iterator vec_t_iter;
    static std::string toString(const Enum en);
    static Enum toEnum(const std::string &en);
};

template <typename EnumMgr, typename Enum>
std::string StringEnum<EnumMgr, Enum>::toString(const Enum en) {
    for (vec_t_iter it = EnumMgr::en2str_vec.begin(); it < EnumMgr::en2str_vec.end(); it++) {
        if (en == it->first)
            return it->second;
    }
    return "";
}

template <typename EnumMgr, typename Enum>
Enum StringEnum<EnumMgr, Enum>::toEnum(const std::string &en) {
    for (vec_t_iter it = EnumMgr::en2str_vec.begin(); it < EnumMgr::en2str_vec.end(); it++) {
        if (en == it->second)
            return it->first;
    }
    return (Enum)0;
}

} /* namespace base */
