#include "tracer/material/lambertian.h"

namespace tracer {
namespace material {

bool Lambertian::scatter(const Ray &r, const hit_record &rec,
                         scatter_record &srec) const {
  srec.is_specular = false;
  srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
  srec.pdf_ptr = std::make_unique<Cosine_pdf>(Cosine_pdf(rec.normal));
  return true;
}

Vec3 Lambertian::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                const Ray &scattered) const {
  auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
  if (cosine < 0)
    return Vec3(0, 0, 0);

  // 获取纹理颜色 (Albedo)
  Vec3 color = albedo->value(rec.u, rec.v, rec.p);

  // 返回 BRDF * cos(theta) = (color / PI) * cos
  return color * (cosine / tracer::math::TRACER_PI);
}

} // namespace material
} // namespace tracer