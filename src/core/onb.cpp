#include "tracer/core/onb.h"

namespace tracer {

Vec3 onb::local(float a, float b, float c) const {
  return a * u() + b * v() + c * w();
}

Vec3 onb::local(const Vec3 &a) const {
  return a.x() * u() + a.y() * v() + a.z() * w();
}

void onb::build_from_w(const Vec3 &n) {
  axis[2] = unit_vector(n);
  Vec3 a = (fabs(w().x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
  axis[1] = unit_vector(cross(w(), a));
  axis[0] = cross(w(), v());
}

} // namespace tracer