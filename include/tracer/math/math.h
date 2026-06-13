#pragma once
#include <cmath>
#include <limits>

namespace tracer {
namespace math {

inline constexpr float TRACER_PI = 3.1415826535897932385f;
inline const float INF = std::numeric_limits<float>::infinity();

template <typename T> inline T mix(const T &a, const T &b, float t) {
  return a * (1.0f - t) + b * t;
}

} // namespace math
} // namespace tracer