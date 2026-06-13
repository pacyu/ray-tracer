#pragma once
#include "tracer/core/material.h"

namespace tracer {
namespace material {

class SunGlow : public Material {
public:
  // 参数：光晕颜色，光晕强度（可调得很大），边缘衰减指数（指数越大，光晕越紧凑）
  SunGlow(const Color &glow_color, float intensity = 5.0f, float falloff = 4.0f)
      : glow_color(glow_color), intensity(intensity), falloff(falloff) {}

  Color emitted(const Ray &r_in, const hit_record &rec, float u, float v,
                const Point3 &p) const override;

  bool scatter(const Ray &r_in, const hit_record &rec,
               scatter_record &srec) const override;

private:
  Color glow_color;
  float intensity;
  float falloff;
};

} // namespace material
} // namespace tracer