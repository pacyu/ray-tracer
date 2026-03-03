#pragma once

#include "vec.h"

class Texture {
public:
	virtual Color value(float u, float v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
public:
	SolidColor() {}
	SolidColor(Color c) : color_value(c) {}
	SolidColor(float red, float green, float blue)
		: SolidColor(Color(red, green, blue)) {}
	virtual Color value(float u, float v, const Vec3& p) const {
		return color_value;
	}

private:
	Color color_value;
};
