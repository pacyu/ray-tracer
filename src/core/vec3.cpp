#include "tracer/core/vec3.h"

namespace tracer {

Vec3 &Vec3::operator+=(const Vec3 &v) {
  e[0] += v.e[0];
  e[1] += v.e[1];
  e[2] += v.e[2];
  return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &v) {
  e[0] -= v.e[0];
  e[1] -= v.e[1];
  e[2] -= v.e[2];
  return *this;
}

Vec3 &Vec3::operator*=(float t) {
  e[0] *= t;
  e[1] *= t;
  e[2] *= t;
  return *this;
}

Vec3 &Vec3::operator/=(float t) { return *this *= (1. / t); }

float Vec3::dot(const Vec3 &v) const {
  return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
}

Vec3 Vec3::cross(const Vec3 &v) const {
  return Vec3(e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2],
              e[0] * v.e[1] - e[1] * v.e[0]);
}

Vec3 Vec3::random() {
  return Vec3(utils::random_float(), utils::random_float(),
              utils::random_float());
}

Vec3 Vec3::random(float min, float max) {
  return Vec3(utils::random_float(min, max), utils::random_float(min, max),
              utils::random_float(min, max));
}

} // namespace tracer