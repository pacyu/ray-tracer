#include "tracer/material/lambertian.h"

namespace tracer {
namespace material {

bool Lambertian::scatter(const Ray &r, const hit_record &rec,
                         scatter_record &srec) const {
  srec.is_specular = false;
  srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
  srec.pdf_ptr = std::make_shared<Cosine_pdf>(rec.normal);
  return true;
}

float Lambertian::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                 const scatter_record &srec,
                                 const Ray &scattered) const {
  float cosine = dot(rec.normal, unit_vector(scattered.direction()));
  if (cosine < 0.0f)
    return 0.0f;

  // 返回 BRDF * cos(theta) = (color / PI) * cos
  return cosine / tracer::math::TRACER_PI;
}

} // namespace material
} // namespace tracer