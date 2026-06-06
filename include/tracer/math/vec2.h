#pragma once

namespace tracer {

struct Vec2 {
  Vec2() {}
  Vec2(float x, float y) : e{x, y} {}

  float e[2] = { 0.0f, 0.0f };

  float x() { return e[0]; }

  float y() { return e[1]; }
};

} // namespace tracer