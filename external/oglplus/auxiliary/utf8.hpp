/**
 *  .file oglplus/auxiliary/utf8.hpp
 *  .brief Helper utf8-related tools
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2011-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __OGLPLUS_AUX_UTF8_1102101236_HPP
#define __OGLPLUS_AUX_UTF8_1102101236_HPP

#if !OGLPLUS_NO_UTF8_CHECKS
#include <oglplus/auxiliary/utf8/validator.hpp>
#endif

namespace oglplus {
namespace aux {

template <typename Iterator>
inline bool ValidUTF8(Iterator begin, Iterator end)
{
#if !OGLPLUS_NO_UTF8_CHECKS
	UTF8Validator<Iterator> valid_utf8;
	return valid_utf8(begin, end);
#else
	OGLPLUS_FAKE_USE(begin);
	OGLPLUS_FAKE_USE(end);
	return true;
#endif
}

} // namespace aux
} // namespace oglplus

#endif
