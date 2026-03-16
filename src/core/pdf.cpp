#include "tracer/core/pdf.h"

namespace tracer {

Vec3 random_cosine_direction() {
  auto r1 = utils::random_float();
  auto r2 = utils::random_float();
  auto z = sqrt(1 - r2);

  auto phi = 2 * utils::TRACER_PI * r1;
  auto x = cos(phi) * sqrt(r2);
  auto y = sin(phi) * sqrt(r2);

  return Vec3(x, y, z);
}

Cosine_pdf::Cosine_pdf(const Vec3 &w) { uvw.build_from_w(w); }

float Cosine_pdf::value(const Vec3 &direction) const {
  auto cosine = dot(unit_vector(direction), uvw.w());
  return (cosine <= 0) ? 0 : cosine / utils::TRACER_PI;
}

Vec3 Cosine_pdf::generate() const {
  return uvw.local(random_cosine_direction());
}

Mixture_pdf::Mixture_pdf(const PDF *p0, const PDF *p1) {
  p[0] = p0;
  p[1] = p1;
}

float Mixture_pdf::value(const Vec3 &direction) const {
  if (!p[0] || !p[1])
    return 0.0f;
  return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
}

Vec3 Mixture_pdf::generate() const {
  if (utils::random_float() < 0.5)
    return p[0]->generate();
  else
    return p[1]->generate();
}

Hittable_pdf::Hittable_pdf(const hittable *p, const Point3 &origin)
    : ptr(p), o(origin) {}

float Hittable_pdf::value(const Vec3 &direction) const {
  return ptr->pdf_value(o, direction);
}

Vec3 Hittable_pdf::generate() const { return ptr->random(o); }

float Sphere_pdf::value(const Vec3 &direction) const {
  return 1 / (4 * tracer::utils::TRACER_PI);
}

Vec3 Sphere_pdf::generate() const { return random_unit_vector(); }

} // namespace tracer