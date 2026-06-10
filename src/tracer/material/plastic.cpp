#include "tracer/material/plastic.h"

namespace tracer {
namespace material {

bool Plastic::scatter(const Ray &r_in, const hit_record &rec,
                      scatter_record &srec) const {
  // 计算菲涅尔系数 (Schlick近似)
  float cos_theta =
      std::min(dot(-unit_vector(r_in.direction()), rec.normal), 1.0f);
  float F0 = pow((1.0f - ior) / (1.0f + ior), 2.0f);
  float fresnel = F0 + (1.0f - F0) * pow(1.0f - cos_theta, 5.0f);

  // 决定这根光线是发生“涂层反射”还是“进入基底散射”
  if (tracer::math::random_float() < fresnel) {
    // 镜面/涂层反射 (Specular/Coat)
    srec.is_specular = true;
    Vec3 reflected =
        tracer::optics::reflect(unit_vector(r_in.direction()), rec.normal);
    // 如果 roughness > 0，给反射方向加一点扰动
    srec.specular_ray = Ray(
        rec.p, reflected + roughness * tracer::math::random_in_unit_sphere());
    srec.attenuation = Color(1.0f, 1.0f, 1.0f); // 涂层反射通常不带色
  } else {
    // 基底漫反射 (Diffuse/Base)
    srec.is_specular = false;
    srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);
  }
  return true;
}

float Plastic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const scatter_record &srec,
                              const Ray &scattered) const {
  if (srec.is_specular)
    return 0.0f;

  float cos_theta = dot(rec.normal, normalize(scattered.direction()));
  if (cos_theta < 0.0f)
    return 0.0f;

  // 重新计算菲涅尔因子（入射角）
  float cos_in =
      std::min(dot(-unit_vector(r_in.direction()), rec.normal), 1.0f);
  float F0 = pow((1.0f - ior) / (1.0f + ior), 2.0f);
  float fresnel = F0 + (1.0f - F0) * pow(1.0f - cos_in, 5.0f);

  // PDF = (1 - fresnel) * (cosθ/π)
  float pdf_val = (1.0f - fresnel) * (cos_theta / tracer::math::TRACER_PI);
  return pdf_val;
}

} // namespace material
} // namespace tracer