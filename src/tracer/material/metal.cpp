#include "tracer/material/metal.h"

namespace tracer {
namespace material {

bool Metal::scatter(const Ray &r, const hit_record &rec,
                    scatter_record &srec) const {
  Vec3 reflected =
      tracer::optics::reflect(unit_vector(r.direction()), rec.normal);
  srec.specular_ray =
      Ray(rec.p, reflected + fuzz * tracer::math::random_in_unit_sphere());
  srec.attenuation = albedo;
  srec.is_specular = true;
  srec.pdf_ptr = nullptr;
  return true;
}

} // namespace material
} // namespace tracer