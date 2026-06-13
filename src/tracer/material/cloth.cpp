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
  srec.is_specular = false;
  srec.attenuation = albedo->value(rec.u, rec.v, rec.p);

  Vec3 view_dir = -unit_vector(r_in.direction());

  std::shared_ptr<Charlie_pdf> charlie_pdf =
      std::make_shared<Charlie_pdf>(rec.normal, view_dir, roughness, rec.tangent, rec.bitangent);
  std::shared_ptr<Cosine_pdf> cos_pdf = std::make_shared<Cosine_pdf>(rec.normal);

  srec.pdf_ptr = std::make_shared<Mixture_pdf>(charlie_pdf, cos_pdf, 0.9f);
  return true;
}

float Cloth::scattering_pdf(const Ray &r_in, const hit_record &rec,
                            const scatter_record &srec,
                            const Ray &scattered) const {
  if (srec.is_specular)
    return 0.0f;

  Vec3 scattered_dir = unit_vector(scattered.direction());
  float cosine = dot(rec.normal, scattered_dir);
  if (cosine <= 0.0f)
    return 0.0f;

  // 1. 漫反射 BRDF = Albedo / PI (Albedo 已经在 attenuation 里了，这里只留
  // 1/PI)
  float diffuse_brdf = 1.0f / tracer::math::TRACER_PI;

  // 2. 布料高光 BRDF
  float charlie = charlie_brdf(r_in, scattered, rec.normal, roughness);

  // 3. 按照你在 scatter 里面设置的 9:1 比例混合 BRDF
  float mixed_brdf = 0.9f * charlie + 0.1f * diffuse_brdf;

  // 4. 严格返回 BRDF * cos(theta)
  return mixed_brdf * cosine;
}

} // namespace material
} // namespace tracer