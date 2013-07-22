#pragma once

#include "vect.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace cp {
	/// Axis-aligned 2D bounding box type
	class BB {
	public:
		BB(Float l, Float b, Float r, Float t) : l(l), b(b), r(r), t(t) {};
		/// left
		Float l;
		/// bottom
		Float b;
		/// right
		Float r;
		/// top
		Float t;

		/// Convenience constructor for making a cp::BB fitting a circle at position p with radius r.
		static BB forCircle(Vect p, Float r);

		/// Returns true if bb contains v.
		bool contains(Vect v) const;
	};
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif