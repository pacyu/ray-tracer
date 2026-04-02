#pragma once
#include "tracer/math/vec3.h"

namespace tracer {

class onb {
public:
  onb() {}

  Vec3 operator[](int i) const { return axis[i]; }

  Vec3 u() const { return axis[0]; }
  Vec3 v() const { return axis[1]; }
  Vec3 w() const { return axis[2]; }

  Vec3 local(float a, float b, float c) const;

  Vec3 local(const Vec3 &a) const;

  void build_from_w(const Vec3 &);

public:
  Vec3 axis[3];
};

} // namespace tracer