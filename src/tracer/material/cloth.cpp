#include "tracer/material/cloth.h"

namespace tracer {
namespace material {

float charlie_brdf(const Ray &r_in, const Ray &scattered, const Vec3 &normal,
                   float roughness) {
  Vec3 v = unit_vector(-r_in.direction());
  Vec3 l = unit_vector(scattered.direction());
  Vec3 h = unit_vector(v + l); // 半程向量

  float ndoth = dot(normal, h);
  float sin_theta = std::sqrt(std::max(0.0f, 1.0f - ndoth * ndoth));

  // 简单的 Charlie 分布近似
  float inv_alpha = 1.0f / roughness;
  return (2.0f + inv_alpha) * std::pow(sin_theta, inv_alpha) /
         (2.0f * tracer::math::TRACER_PI);
}

bool Cloth::scatter(const Ray &r_in, const hit_record &rec,
                    scatter_record &srec) const {
  // 1. 采样半程向量 (Half-vector) 遵循 Charlie 分布
  // 2. 计算边缘泛光强度
  // 3. 生成散射光线
  // 简单实现可以采用 Cosine Sampling，但在颜色计算时叠加 Sheen 项
  srec.is_specular = false; // 布料通常不是纯镜面反射
  srec.attenuation = albedo;
  srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);
  return true;
}

Color Cloth::scattering_pdf(const Ray &r_in, const hit_record &rec,
                            const Ray &scattered) const {
  auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
  if (cosine < 0)
    return Color(0, 0, 0);

  // 计算 Charlie Sheen 贡献
  float sheen_strength = charlie_brdf(r_in, scattered, rec.normal, roughness);

  return albedo * (cosine / tracer::math::TRACER_PI) + sheen_strength;
}

} // namespace material
} // namespace tracer