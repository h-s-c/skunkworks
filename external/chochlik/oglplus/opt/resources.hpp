/**
 *  @file oglplus/opt/resources.hpp
 *  @brief Resource (texture, model, etc.) file find functions
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_OPT_RESOURCES_1107121519_HPP
#define OGLPLUS_OPT_RESOURCES_1107121519_HPP

#include <oglplus/config.hpp>
#include <oglplus/string.hpp>
#include <oglplus/opt/application.hpp>
#include <oglplus/auxiliary/filesystem.hpp>

#include <fstream>
#include <cassert>

namespace oglplus {
namespace aux {

inline std::size_t FindResourceFile(
	std::ifstream& file,
	const std::string& path,
	const char** exts,
	std::size_t nexts
)
{
	for(std::size_t e=0; e!=nexts; ++e)
	{
		file.open(path + exts[e], std::ios::binary);
		if(file.good()) return e;
	}
	return nexts;
}

} // namespace aux

inline std::size_t FindResourceFile(
	std::ifstream& file,
	const std::string& category,
	const std::string& name,
	const char** exts,
	unsigned nexts
)
{
	const std::string dirsep = aux::FilesysPathSep();
	const std::string pardir(aux::FilesysPathParDir() + dirsep);
	const std::string path = category+dirsep+name;
	const std::string apppath = Application::RelativePath();
	std::string prefix;

	for(std::size_t i=0; i!=5; ++i)
	{
		std::size_t iext = aux::FindResourceFile(
			file,
			apppath+prefix+path,
			exts,
			nexts
		);
		if(iext != nexts) return iext;
		prefix = pardir + prefix;
	}
	return nexts;
}

inline bool OpenResourceFile(
	std::ifstream& file,
	const std::string& category,
	const std::string& name,
	const char* ext
)
{
	return FindResourceFile(
		file,
		category,
		name,
		&ext,
		1
	) == 0;
}

} // namespace oglplus

#endif // include guard
