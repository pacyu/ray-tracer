#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

namespace tracer {

namespace utils {

inline constexpr float TRACER_PI = 3.1415826535897932385f;
inline const float inf = std::numeric_limits<float>::infinity();

inline float clamp(float x, float min, float max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

} // namespace utils

} // namespace tracer