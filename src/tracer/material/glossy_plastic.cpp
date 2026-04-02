#include "tracer/material/glossy_plastic.h"

namespace tracer {
namespace material {

bool GlossyPlastic::scatter(const Ray &r_in, const hit_record &rec,
                            scatter_record &srec) const {
  float F =
      std::clamp(tracer::optics::schlick(
                     dot(unit_vector(-r_in.direction()), rec.normal), ior),
                 0.0f, 1.0f);
  if (tracer::math::random_float() < F) {
    // 1. 镜面反射逻辑
    Vec3 reflected =
        tracer::optics::reflect(unit_vector(r_in.direction()), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected);
    srec.is_specular = true;
    srec.attenuation = Color(1, 1, 1);
  } else {
    srec.is_specular = false;
    srec.attenuation = albedo;
    srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);
  }
  return true;
}

Vec3 GlossyPlastic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                   const Ray &scattered) const {
  float cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
  if (cos_theta < 0.0f)
    return Vec3(0.0f, 0.0f, 0.0f);

  float F =
      std::clamp(tracer::optics::schlick(
                     dot(unit_vector(-r_in.direction()), rec.normal), ior),
                 0.0f, 1.0f);

  // 混合 PDF：漫反射部分 (Cosine PDF) + 镜面部分
  // 注意：如果是完美镜面，镜面部分的 PDF 是 Delta 分布，通常不直接加在 PDF
  // 函数里 这里返回漫反射部分的 PDF 贡献
  return (1.0f - F) * albedo;
}

} // namespace material
} // namespace tracer