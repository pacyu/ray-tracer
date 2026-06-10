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

  bool tangent_valid = (rec.tangent.squared_length() > 0.5f);
  if (roughness > 0.01f && tangent_valid) {
    srec.pdf_ptr = std::make_unique<Charlie_pdf>(
        rec.normal, view_dir, roughness, rec.tangent, rec.bitangent);
  } else {
    srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);
  }
  return true;
}

float Cloth::scattering_pdf(const Ray &r_in, const hit_record &rec,
                            const scatter_record &srec,
                            const Ray &scattered) const {
  if (srec.is_specular)
    return 0.0f;
  if (srec.pdf_ptr)
    return srec.pdf_ptr->value(scattered.direction());
  return 0.0f;
}

} // namespace material
} // namespace tracer