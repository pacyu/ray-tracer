#include "tracer/texture/solid_color.h"

namespace tracer {
namespace texture {

Color SolidColor::value(float u, float v, const Vec3 &p) const {
  return color_value;
}

} // namespace texture
} // namespace tracer