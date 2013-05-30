/**
 *  @file oglplus/auto_bind.hpp
 *  @brief OGLplus AutoBind objects
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_AUTO_BIND_1201190831_HPP
#define OGLPLUS_AUTO_BIND_1201190831_HPP

#include <oglplus/bound.hpp>
#include <stack>

namespace oglplus {

template <class Bindable>
class AutoBindBase
 : public Bindable
 , public FriendOf<Bindable>
{
private:
	typedef typename Bindable::Target Target;

	static bool _is_bound(
		const Bindable& bindable,
		Target target
	)
	{
		typedef typename ObjectBaseOps<Bindable>::Type BindableOps;
		auto name = FriendOf<Bindable>::GetName(bindable);
		auto sname = BindingQuery<BindableOps>::QueryBinding(target);
		return name == sname;
	}

	static void _bind_if_necessary(
		const Bindable& bindable,
		Target target
	)
	{
		if(!_is_bound(bindable, target))
			bindable.Bind(target);
	}

	Target _bind_target;
protected:
	AutoBindBase(Target target)
	 : _bind_target(target)
	{ }
public:
	void Bind(void) const
	{
		_bind_if_necessary(*this, _bind_target);
	}

	typename Bindable::Target BindTarget(void) const
	{
		_bind_if_necessary(*this, _bind_target);
		return _bind_target;
	}
};

template <class Bindable>
class AutoBind;

#if OGLPLUS_DOCUMENTATION_ONLY
/// A wraper that automatically binds @ref oglplus_object "objects" to a target
/**
 *  @ref bound_objects make the usage of @ref oglplus_object "objects" that
 *  can be bound to a OpenGL binding point or "target" easier.
 *  This includes objects like @ref oglplus::Buffer "Buffer,"
 *  @ref oglplus::Texture "Texture", @ref oglplus::Renderbuffer "Renderbuffer"
 *  or @ref oglplus::Framebuffer "Framebuffer"
 *  which have a target to which individual
 *  instances can be bound and operated on through the binding point.
 *
 *  @ingroup utility_classes
 */
template <class Bindable>
class AutoBind
 : public Bindable
{
public:
	/// Creates a @c Bindable so that it binds to a @c target when necessary
	/**
	 *  @see Bound
	 */
	AutoBind(typename Bindable::Target target);
};
#else
template <class Bindable>
class AutoBind<Object<Bindable> >
 : public BoundTemplate<AutoBindBase, Object<Bindable>, Bindable>
{
public:
	AutoBind(typename Bindable::Target target)
	 : BoundTemplate<AutoBindBase, Object<Bindable>, Bindable>(target)
	{ }
};
#endif

} // namespace oglplus

#endif // include guard
