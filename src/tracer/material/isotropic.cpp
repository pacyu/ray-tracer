#include "tracer/material/isotropic.h"

namespace tracer {
namespace material {

bool Isotropic::scatter(const Ray &r_in, const hit_record &rec,
                        scatter_record &srec) const {
  srec.attenuation = tex->value(rec.u, rec.v, rec.p);
  srec.pdf_ptr = std::make_unique<Sphere_pdf>();
  srec.is_specular = false;
  return true;
}

Vec3 Isotropic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const {
  Vec3 color = tex->value(rec.u, rec.v, rec.p);
  return color * (1.0f / (4.0f * tracer::math::TRACER_PI));
}

} // namespace material
} // namespace tracer