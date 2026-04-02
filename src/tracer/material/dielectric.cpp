#include "tracer/material/dielectric.h"

namespace tracer {
namespace material {

bool Dielectric::scatter(const Ray &r, const hit_record &rec,
                         scatter_record &srec) const {
  srec.is_specular = true;
  srec.pdf_ptr = nullptr;

  if (!rec.front_face) {
    // 计算指数衰减：exp(-density * distance)
    float r = std::exp(-density * (1.0f - albedo.r()) * rec.t);
    float g = std::exp(-density * (1.0f - albedo.g()) * rec.t);
    float b = std::exp(-density * (1.0f - albedo.b()) * rec.t);
    srec.attenuation = Color(r, g, b);
  } else {
    srec.attenuation = Color(1.0f, 1.0f, 1.0f);
  }

  float etai_over_etat = rec.front_face ? (1.f / ref_idx) : ref_idx;
  Vec3 unit_direction = unit_vector(r.direction());
  float cos_theta = std::min(dot(-unit_direction, rec.normal), 1.f);
  float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);

  float offset_epsilon = 0.001f;
  float reflect_prob = tracer::optics::schlick(cos_theta, etai_over_etat);
  if (etai_over_etat * sin_theta > 1.f ||
      reflect_prob > tracer::math::random_float()) {
    Vec3 reflected = tracer::optics::reflect(unit_direction, rec.normal);
    srec.specular_ray = Ray(rec.p + offset_epsilon * rec.normal, reflected);
  } else { // 折射
    Vec3 refracted =
        tracer::optics::refract(unit_direction, rec.normal, etai_over_etat);
    srec.specular_ray = Ray(rec.p - offset_epsilon * rec.normal, refracted);
  }
  return true;
}

} // namespace material
} // namespace tracer