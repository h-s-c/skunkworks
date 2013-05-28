/**
 *  @file oglplus/imports/blend_file/flattened.hpp
 *  @brief Classes handling flattened SDNA structures
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_IMPORTS_BLEND_FILE_FLATTENED_1107121519_HPP
#define OGLPLUS_IMPORTS_BLEND_FILE_FLATTENED_1107121519_HPP

namespace oglplus {
namespace imports {


class BlendFileFlattenedStructField
{
private:
	BlendFileSDNA* _sdna;
	std::size_t _struct_index;
	std::size_t _flat_field_index;
	const BlendFileSDNA::_flat_struct_info* _flat_fields;

	BlendFileFlattenedStructField(
		BlendFileSDNA* sdna,
		std::size_t struct_index,
		std::size_t flat_field_index,
		const BlendFileSDNA::_flat_struct_info* flat_fields
	): _sdna(sdna)
	 , _struct_index(struct_index)
	 , _flat_field_index(flat_field_index)
	 , _flat_fields(flat_fields)
	{ }

	friend class BlendFileBlockData;
	friend class BlendFileFlattenedStruct;
	friend class BlendFileFlattenedStructFieldRange;
public:

	const std::string& Name(void) const;

	BlendFileStruct Parent(void) const;

	BlendFileStructField Field(void) const;

	uint32_t Offset(void) const;

	uint32_t Size(void) const
	{
		return Field().Size();
	}
};

#if !OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)

OGLPLUS_LIB_FUNC
const std::string& BlendFileFlattenedStructField::Name(void) const
{
	return _flat_fields->_field_names[_flat_field_index];
}

OGLPLUS_LIB_FUNC
BlendFileStruct BlendFileFlattenedStructField::Parent(void) const
{
	return BlendFileStruct(
		_sdna,
		_flat_fields->_field_structs[_flat_field_index]
	);
}

OGLPLUS_LIB_FUNC
BlendFileStructField BlendFileFlattenedStructField::Field(void) const
{
	return BlendFileStructField(
		_sdna,
		_flat_fields->_field_structs[_flat_field_index],
		_flat_fields->_field_indices[_flat_field_index]
	);
}

OGLPLUS_LIB_FUNC
uint32_t BlendFileFlattenedStructField::Offset(void) const
{
	return _flat_fields->_field_offsets[_flat_field_index];
}

#endif // OGLPLUS_LINK_LIBRARY

class BlendFileFlattenedStructFieldRange
 : public BlendFileRangeTpl<
	BlendFileFlattenedStructFieldRange,
	BlendFileFlattenedStructField
>
{
private:
	BlendFileSDNA* _sdna;
	std::size_t _struct_index;
	const BlendFileSDNA::_flat_struct_info* _flat_fields;

	typedef BlendFileRangeTpl<
		BlendFileFlattenedStructFieldRange,
		BlendFileFlattenedStructField
	> Base;

	static std::size_t _field_count(
		const BlendFileSDNA::_flat_struct_info* flat_fields
	)
	{
		if(flat_fields) return flat_fields->_field_count();
		else return 0;
	}

	BlendFileFlattenedStructFieldRange(
		BlendFileSDNA* sdna,
		std::size_t struct_index,
		const BlendFileSDNA::_flat_struct_info* flat_fields
	): Base(_field_count(flat_fields))
	 , _sdna(sdna)
	 , _struct_index(struct_index)
	 , _flat_fields(flat_fields)
	{ }

	friend class BlendFileFlattenedStruct;
public:
	BlendFileFlattenedStructField Get(std::size_t index) const
	{
		assert(_flat_fields);
		return BlendFileFlattenedStructField(
			_sdna,
			_struct_index,
			index,
			_flat_fields
		);
	}
};

class BlendFileFlattenedStruct
 : public BlendFileType
{
public:
	BlendFileFlattenedStruct(const BlendFileType& type)
	 : BlendFileType(type)
	{ }

	/// Returns a range of fields of the flattened structure
	BlendFileFlattenedStructFieldRange Fields(void) const;

	/// Returns a field by its full name
	BlendFileFlattenedStructField FieldByName(const std::string& name) const;
};

BlendFileFlattenedStruct BlendFileStruct::Flattened(void) const
{
	return BlendFileFlattenedStruct(*this);
}

#if !OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)

OGLPLUS_LIB_FUNC
BlendFileFlattenedStructFieldRange
BlendFileFlattenedStruct::Fields(void) const
{
	const BlendFileSDNA::_flat_struct_info* flat_fields =
		(_struct_index == _sdna->_invalid_struct_index())?
		(const BlendFileSDNA::_flat_struct_info*)nullptr:
		(_sdna->_struct_flatten_fields(_struct_index).get());

	return BlendFileFlattenedStructFieldRange(
		_sdna,
		_struct_index,
		flat_fields
	);
}

OGLPLUS_LIB_FUNC
BlendFileFlattenedStructField
BlendFileFlattenedStruct::FieldByName(const std::string& name) const
{
	// this of course does not work for atomic types
	if(_struct_index == _sdna->_invalid_struct_index())
	{
		std::string what("Requesting field '");
		what.append(name);
		what.append("' in an atomic type");
		throw std::runtime_error(what);
	}

	const BlendFileSDNA::_flat_struct_info* flat_fields =
		_sdna->_struct_flatten_fields(_struct_index).get();
	assert(flat_fields);

	auto pos = flat_fields->_field_map.find(&name);

	if(pos == flat_fields->_field_map.end())
	{
		std::string what("Cannot find field '");
		what.append(name);
		what.append("' in flattened structure");
		throw std::runtime_error(what);
	}

	const std::size_t flat_field_index = pos->second;

	return BlendFileFlattenedStructField(
		_sdna,
		_struct_index,
		flat_field_index,
		flat_fields
	);
}

#endif // OGLPLUS_LINK_LIBRARY

} // imports
} // oglplus

#endif // include guard
