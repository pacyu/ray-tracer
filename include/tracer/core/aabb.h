#pragma once
#include "tracer/core/ray.h"

namespace tracer {

class AABB {
public:
  AABB() {}
  AABB(const Point3 &a, const Point3 &b) : min(a), max(b) {}

  bool hit(const Ray &r, float tmin, float tmax) const;
  static AABB surrounding_box(AABB box0, AABB box1);

  void expand(const Vec3 &point);

  void expand(const AABB &box);

  int max_extent() const;

  Point3 min, max;
};

} // namespace tracer
