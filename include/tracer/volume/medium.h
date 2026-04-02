#pragma once
#include "tracer/math/vec3.h"

namespace tracer {

class Medium {
public:
  Vec3 sigma_a; // absorption
  Vec3 sigma_s; // scattering
  float g;      // HG parameter

  Medium(const Vec3 &a, const Vec3 &s, float g)
      : sigma_a(a), sigma_s(s), g(g) {}

  Vec3 sigma_t() const { return sigma_a + sigma_s; }
};

} // namespace tracer