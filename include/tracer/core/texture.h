#pragma once
#include "opencv2/opencv.hpp"
#include "tracer/math/vec3.h"

namespace tracer {

class Texture {
public:
  virtual Color value(float u, float v, const Point3 &p) const = 0;
};

} // namespace tracer