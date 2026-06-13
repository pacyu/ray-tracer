#pragma once

namespace tracer {

struct Vec2 {
  Vec2() {}
  Vec2(float x, float y) : e{x, y} {}

  float e[2] = {0.0f, 0.0f};

  const float x() const { return e[0]; }

  const float y() const { return e[1]; }
};

inline Vec2 operator+(const Vec2 &v1, const Vec2 &v2) {
  return Vec2(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1]);
}

inline Vec2 operator*(float t, const Vec2 &v) {
  return Vec2(t * v.e[0], t * v.e[1]);
}

inline Vec2 operator-(const Vec2 &v1, const Vec2 &v2) {
  return Vec2(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1]);
}

} // namespace tracer