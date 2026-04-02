#include "tracer/material/diffuse_light.h"

namespace tracer {
namespace material {

bool DiffuseLight::scatter(const Ray &r_in, const hit_record &rec,
                           scatter_record &srec) const {
  return false;
}

Color DiffuseLight::emitted(const Ray &r_in, const hit_record &rec, float u,
                            float v, const Point3 &p) const {
  return emit->value(u, v, p);
}

} // namespace material
} // namespace tracer