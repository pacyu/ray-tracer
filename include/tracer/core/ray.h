#pragma once
#include "tracer/core/vec3.h"

namespace tracer {

class Ray {
public:
  Ray() : orig(), dir() {}
  Ray(const Point3 &origin, const Point3 &dir) : orig(origin), dir(dir) {}
  Ray(const Point3 &origin, const Vec3 &direction, float time)
      : orig(origin), dir(direction), tm(time) {}
  Point3 origin() const { return orig; }
  Point3 direction() const { return dir; }
  Point3 at(float t) const { return orig + t * dir; }
  float time() const { return tm; }

private:
  Point3 orig;
  Point3 dir;
  float tm;
};

} // namespace tracer