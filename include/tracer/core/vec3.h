#pragma once
#include "tracer/utils/drand48.h"
#include "tracer/utils/math.h"
#include <iostream>
#include <smmintrin.h>

namespace tracer {

class Vec3 {
public:
  Vec3() : e() {}
  Vec3(float e0, float e1, float e2) : e{e0, e1, e2} {}
  float x() const { return e[0]; }
  float y() const { return e[1]; }
  float z() const { return e[2]; }
  float r() const { return e[0]; }
  float g() const { return e[1]; }
  float b() const { return e[2]; }

  Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
  float operator[](unsigned i) const { return e[i]; }
  float &operator[](unsigned i) { return e[i]; }

  Vec3 &operator+=(const Vec3 &v);
  Vec3 &operator-=(const Vec3 &v);
  Vec3 &operator*=(float t);
  Vec3 &operator/=(float t);
  bool operator==(const Vec3 &v) const {
    return e[0] == v.e[0] && e[1] == v.e[1] && e[2] == v.e[2];
  }
  float dot(const Vec3 &v) const;
  Vec3 cross(const Vec3 &v) const;

  float length() const { return std::sqrt(squared_length()); }

  float squared_length() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  static Vec3 random();

  static Vec3 random(float min, float max);

public:
  float e[3];
};

inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
  return os << '(' << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ')';
}

inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline Vec3 operator*(float t, const Vec3 &v) {
  return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, float t) { return t * v; }

inline Vec3 operator/(const Vec3 &v, float t) { return (1. / t) * v; }

inline float dot(const Vec3 &u, const Vec3 &v) {
  __m128 m1 = _mm_set_ps(0, u.e[2], u.e[1], u.e[0]);
  __m128 m2 = _mm_set_ps(0, v.e[2], v.e[1], v.e[0]);
  __m128 res = _mm_dp_ps(m1, m2, 0x71);
  return _mm_cvtss_f32(res);
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(const Vec3 &v) { return v / v.length(); }

inline Vec3 random_unit_vector() {
  float a = utils::random_float(0., 2 * utils::TRACER_PI),
        z = utils::random_float(-1., 1.), r = std::sqrt(1 - z * z);
  return Vec3(r * std::cos(a), r * std::sin(a), z);
}

inline Vec3 random_to_sphere(float radius, float distance_squared) {
  float r1 = utils::random_float();
  float r2 = utils::random_float();
  float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

  float phi = 2 * utils::TRACER_PI * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  auto y = sin(phi) * sqrt(1 - z * z);

  return Vec3(x, y, z);
}

using Point3 = Vec3;
using Color = Vec3;

} // namespace tracer
