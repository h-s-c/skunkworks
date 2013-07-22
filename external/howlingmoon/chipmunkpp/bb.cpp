#include "bb.hpp"

namespace cp {
	BB BB::forCircle(Vect p, Float r) {
		auto bb = BB(p.x - r,
		          p.y - r,
		          p.x + r,
		          p.y + r);
		return bb;
	}

	bool BB::contains(Vect v) const {
		return (l <= v.x && r >= v.x && b <= v.y && t >= v.y);
	}
}