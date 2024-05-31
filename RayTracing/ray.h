#pragma once
#include "vec.h"
class Ray {
public:
	Ray() : orig(), dir() {}
	Ray(const Point3& orig, const Point3& dir)
		: orig(orig), dir(dir)
	{}
	Point3 origin() const { return orig; }
	Point3 direction() const { return dir; }
	Point3 at(float t) const { return orig + t * dir; }

	Point3 orig;
	Point3 dir;
};