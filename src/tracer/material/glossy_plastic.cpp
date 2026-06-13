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
    Vec3 reflected =
        tracer::optics::reflect(unit_vector(r_in.direction()), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected);
    srec.is_specular = true;
    srec.attenuation = Color(1.0f, 1.0f, 1.0f);
  } else {
    srec.is_specular = false;
    srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = std::make_shared<Cosine_pdf>(rec.normal);
  }
  return true;
}

float GlossyPlastic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                    const scatter_record &srec,
                                    const Ray &scattered) const {
  if (srec.is_specular)
    return 0.0f;

  float cos_theta = dot(rec.normal, normalize(scattered.direction()));
  if (cos_theta < 0.0f)
    return 0.0f;

  // 重新计算菲涅尔因子 F
  float cos_in = dot(unit_vector(-r_in.direction()), rec.normal);
  float F = std::clamp(tracer::optics::schlick(cos_in, ior), 0.0f, 1.0f);

  // 漫反射分支的 PDF = (1 - F) * (cosθ / π)
  float pdf_val = (1.0f - F) * (cos_theta / tracer::math::TRACER_PI);
  return pdf_val;
}

} // namespace material
} // namespace tracer