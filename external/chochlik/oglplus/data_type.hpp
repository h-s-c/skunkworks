/**
 *  @file oglplus/data_type.hpp
 *  @brief Data type-related declarations
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_DATA_TYPE_1107121519_HPP
#define OGLPLUS_DATA_TYPE_1107121519_HPP

#include <oglplus/enumerations.hpp>
#include <type_traits>

namespace oglplus {

/// A tag template used mainly for data-type-based function overload dispatching
template <typename GLtype>
struct TypeTag
{ };

/// OpenGL data type enumeration
/**
 *  @ingroup enumerations
 *
 *  @glsymbols
 *  @glfunref{DrawElements}
 *  @glfunref{VertexAttribPointer}
 */
OGLPLUS_ENUM_CLASS_BEGIN(DataType, GLenum)
#include <oglplus/enums/data_type.ipp>
OGLPLUS_ENUM_CLASS_END(DataType)

#if !OGLPLUS_NO_ENUM_VALUE_NAMES
#include <oglplus/enums/data_type_names.ipp>
#endif

#if !OGLPLUS_ENUM_VALUE_RANGES
#include <oglplus/enums/data_type_range.ipp>
#endif

inline DataType GetDataType(GLbyte*)
{
	return DataType::Byte;
}

inline DataType GetDataType(GLshort*)
{
	return DataType::Short;
}

inline DataType GetDataType(GLint*)
{
	return DataType::Int;
}

inline DataType GetDataType(GLubyte*)
{
	return DataType::UnsignedByte;
}

inline DataType GetDataType(GLushort*)
{
	return DataType::UnsignedShort;
}

inline DataType GetDataType(GLuint*)
{
	return DataType::UnsignedInt;
}

inline DataType GetDataType(GLfloat*)
{
	return DataType::Float;
}

inline DataType GetDataType(GLdouble*)
{
	return DataType::Double;
}

template <typename T>
struct IsGLDataType
 : public std::false_type
{ };

template <>
struct IsGLDataType<GLbyte>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLshort>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLint>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLubyte>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLushort>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLuint>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLfloat>
 : public std::true_type
{ };

template <>
struct IsGLDataType<GLdouble>
 : public std::true_type
{ };

/// Returns the DataType for the specified type @p T
template <typename T>
inline DataType GetDataType(void)
{
	return GetDataType((T*)nullptr);
}


/// OpenGL Shading Language data type enumeration
/**
 *  @ingroup enumerations
 */
OGLPLUS_ENUM_CLASS_BEGIN(SLDataType, GLenum)
#include <oglplus/enums/sl_data_type.ipp>
OGLPLUS_ENUM_CLASS_END(SLDataType)

#if !OGLPLUS_NO_ENUM_VALUE_NAMES
#include <oglplus/enums/sl_data_type_names.ipp>
#endif

#if !OGLPLUS_ENUM_VALUE_RANGES
#include <oglplus/enums/sl_data_type_range.ipp>
#endif

} // namespace oglplus

#endif // include guard
