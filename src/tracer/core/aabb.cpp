#include "tracer/core/aabb.h"

namespace tracer {

bool AABB::hit(const Ray &r, float tmin, float tmax) const {
  for (int i = 0; i < 3; i++) {
    float invD = 1.f / r.direction()[i];
    float t0 = (min[i] - r.origin()[i]) * invD,
          t1 = (max[i] - r.origin()[i]) * invD;
    if (invD < 0.0f)
      std::swap(t0, t1);
    tmin = t0 > tmin ? t0 : tmin;
    tmax = t1 < tmax ? t1 : tmax;
    if (tmax <= tmin)
      return false;
  }
  return true;
}

AABB AABB::surrounding_box(AABB box0, AABB box1) {
  return AABB(Point3(std::min(box0.min.x(), box1.min.x()),
                     std::min(box0.min.y(), box1.min.y()),
                     std::min(box0.min.z(), box1.min.z())),
              Point3(std::max(box0.max.x(), box1.max.x()),
                     std::max(box0.max.y(), box1.max.y()),
                     std::max(box0.max.z(), box1.max.z())));
}

void AABB::expand(const Vec3 &point) {
  min = Vec3::min(min, point);
  max = Vec3::max(max, point);
}

void AABB::expand(const AABB &box) {
  expand(box.min);
  expand(box.max);
}

int AABB::max_extent() const {
  Vec3 d = max - min;
  if (d.x() > d.y() && d.x() > d.z())
    return 0;
  if (d.y() > d.z())
    return 1;
  return 2;
}

} // namespace tracer