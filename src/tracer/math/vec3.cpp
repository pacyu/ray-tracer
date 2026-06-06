#include "tracer/math/vec3.h"

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

Vec3 &Vec3::operator/=(float t) { return *this *= (1.f / t); }

float Vec3::dot(const Vec3 &v) const {
  __m128 m1 = _mm_set_ps(0, e[2], e[1], e[0]);
  __m128 m2 = _mm_set_ps(0, v.e[2], v.e[1], v.e[0]);
  __m128 res = _mm_dp_ps(m1, m2, 0x71);
  return _mm_cvtss_f32(res);
}

Vec3 Vec3::cross(const Vec3 &v) const {
  return Vec3(e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2],
              e[0] * v.e[1] - e[1] * v.e[0]);
}

Vec3 Vec3::random() {
  return Vec3(math::random_float(), math::random_float(), math::random_float());
}

Vec3 Vec3::random(float min, float max) {
  return Vec3(math::random_float(min, max), math::random_float(min, max),
              math::random_float(min, max));
}

Vec3 Vec3::min(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(std::min(v1.x(), v2.x()), std::min(v1.y(), v2.y()),
              std::min(v1.z(), v2.z()));
}

Vec3 Vec3::max(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(std::max(v1.x(), v2.x()), std::max(v1.y(), v2.y()),
              std::max(v1.z(), v2.z()));
}

} // namespace tracer