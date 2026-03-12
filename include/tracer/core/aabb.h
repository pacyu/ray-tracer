#pragma once

#include "tracer/core/ray.h"

namespace tracer {

class AABB {
public:
  AABB() {}
  AABB(const Point3 &a, const Point3 &b) : min(a), max(b) {}

  bool hit(const Ray &r, float tmin, float tmax) const;
  static AABB surrounding_box(AABB box0, AABB box1);

  Point3 min, max;
};

} // namespace tracer
