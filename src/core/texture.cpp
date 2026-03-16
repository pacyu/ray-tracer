#include "tracer/core/texture.h"

namespace tracer {

Color SolidColor::value(float u, float v, const Vec3 &p) const {
  return color_value;
}

} // namespace tracer