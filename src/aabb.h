#pragma once

#include "ray.h"

class AABB {
public:
	AABB() {}
	AABB(const Point3& a, const Point3& b)
		: min(a), max(b) {}

	bool hit(const Ray& r, float tmin, float tmax) const {
		for (int i = 0; i < 3; i++)
		{
			float invD = 1. / r.direction()[i];
			float t0 = (min[i] - r.origin()[i]) * invD,
				   t1 = (max[i] - r.origin()[i]) * invD;
			if (invD < 0.) std::swap(t0, t1);
			tmin = std::max(t0, tmin);
			tmax = std::min(t1, tmax);
			if (tmax <= tmin) return false;
		}
		return true;
	}

	Point3 min, max;
};

AABB surrounding_box(AABB box0, AABB box1) {
	return AABB(
		Point3(
			std::min(box0.min.x(), box1.min.x()),
			std::min(box0.min.y(), box1.min.y()),
			std::min(box0.min.z(), box1.min.z())),
		Point3(
			std::max(box0.max.x(), box1.max.x()),
			std::max(box0.max.y(), box1.max.y()),
			std::max(box0.max.z(), box1.max.z())));
}