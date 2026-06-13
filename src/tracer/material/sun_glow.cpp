#include "tracer/material/sun_glow.h"

namespace tracer {
namespace material {

Color SunGlow::emitted(const Ray &r_in, const hit_record &rec, float u, float v,
                       const Point3 &p) const {
  // 计算入射光方向与球面法线的夹角
  // 视线正对太阳中心时，cos_theta = 1（最亮）
  // 视线擦过光晕边缘时，cos_theta = 0（最暗）
  float cos_theta = std::abs(dot(rec.normal, unit_vector(r_in.direction())));

  // 使用 pow 函数形成优美的指数淡出边缘
  float glow_factor = std::pow(cos_theta, falloff);

  return glow_color * intensity * glow_factor;
}

bool SunGlow::scatter(const Ray &r_in, const hit_record &rec,
                      scatter_record &srec) const {
  srec.is_specular = true;
  srec.attenuation = Color(1.0f, 1.0f, 1.0f); // 100% 能量穿透
  srec.pdf_ptr = nullptr;

  // 沿着原射线方向继续前进，稍微加一点 offset 避免自相交死循环
  srec.specular_ray = Ray(rec.p + unit_vector(r_in.direction()) * 1e-3f,
                          r_in.direction(), r_in.time());
  return true;
}

} // namespace material
} // namespace tracer