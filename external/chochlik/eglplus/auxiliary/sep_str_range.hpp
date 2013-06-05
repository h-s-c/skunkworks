/**
 *  .file eglplus/auxiliary/sep_str_range.hpp
 *  .brief Range for traversal of separated string values
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2012-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef EGLPLUS_AUX_SEP_STR_RANGE_1305291314_HPP
#define EGLPLUS_AUX_SEP_STR_RANGE_1305291314_HPP

#include <eglplus/string.hpp>
#include <algorithm>

namespace eglplus {
namespace aux {

class SepStrRange
{
private:
	String _values;
	String::iterator _i, _e;
public:
	typedef String ValueType;

	SepStrRange(const char* str, char sep = ' ')
	 : _values(str)
	 , _i(_values.begin())
	 , _e(_values.end())
	{
		std::replace(_i, _e, sep, '\0');
	}

	bool Empty(void) const
	{
		return _i == _e;
	}

	String Front(void) const
	{
		assert(!Empty());
		String::iterator p = std::find(_i, _e, '\0');
		return String(_i, p);
	}

	void Next(void)
	{
		assert(!Empty());
		_i = std::find(_i, _e, '\0');
		assert(_i != _e);
		++_i;
		if((_i != _e) && (*_i == '\0'))
			_i = _e;
	}
};

} // namespace aux
} // namespace eglplus

#endif // include guard
