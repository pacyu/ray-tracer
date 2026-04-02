#pragma once
#include "tracer/core/texture.h"

namespace tracer {
namespace texture {

class SolidColor : public Texture {
public:
  SolidColor() {}
  SolidColor(Color c) : color_value(c) {}
  SolidColor(float red, float green, float blue)
      : SolidColor(Color(red, green, blue)) {}

  virtual Color value(float u, float v, const Vec3 &p) const override;

private:
  Color color_value;
};

} // namespace texture
} // namespace tracer