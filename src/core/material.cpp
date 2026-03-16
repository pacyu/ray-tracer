#include "tracer/core/material.h"

namespace tracer {

Vec3 random_in_unit_sphere() {
  Vec3 p;
  do {
    p = 2.0f * Vec3(utils::random_float(), utils::random_float(),
                    utils::random_float()) -
        Vec3(1, 1, 1);
  } while (p.squared_length() >= 1.0f);
  return p;
}

Vec3 reflect(const Vec3 &v, const Vec3 &n) { return v - 2 * dot(v, n) * n; }

Vec3 refract(const Vec3 &uv, const Vec3 &n, float etai_over_etat) {
  float cos_theta = dot(-uv, n);
  Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3 r_out_parallel =
      -std::sqrt(std::abs(1. - r_out_perp.squared_length())) * n;
  return r_out_perp + r_out_parallel;
}

float schlick(float cosine, float ref_idx) {
  float r0 = (1. - ref_idx) / (1. + ref_idx);
  r0 *= r0;
  return r0 + (1. - r0) * std::pow((1. - cosine), 5.);
}

bool Lambertian::scatter(const Ray &r, const hit_record &rec,
                         scatter_record &srec) const {
  srec.is_specular = false;
  srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
  srec.pdf_ptr = std::make_unique<Cosine_pdf>(Cosine_pdf(rec.normal));
  return true;
}

float Lambertian::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                 const Ray &scattered) const {
  auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
  return cosine < 0 ? 0 : cosine / utils::TRACER_PI;
}

bool Metal::scatter(const Ray &r, const hit_record &rec,
                    scatter_record &srec) const {
  Vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
  srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
  srec.attenuation = albedo;
  srec.is_specular = true;
  srec.pdf_ptr = 0;
  return true;
}

bool Dielectric::scatter(const Ray &r, const hit_record &rec,
                         scatter_record &srec) const {
  srec.is_specular = true;
  srec.pdf_ptr = nullptr;
  if (!rec.front_face) {
    float distance = rec.t;
    srec.attenuation =
        Color(std::exp(-density * (1.0f - albedo.r()) * distance),
              std::exp(-density * (1.0f - albedo.g()) * distance),
              std::exp(-density * (1.0f - albedo.b()) * distance));
  } else {
    srec.attenuation = Color(1.0, 1.0, 1.0); // 进入时保持透明
  }
  float etai_over_etat = rec.front_face ? (1. / ref_idx) : ref_idx;
  Vec3 unit_direction = unit_vector(r.direction());
  float cos_theta = std::min(dot(-unit_direction, rec.normal), 1.f);
  float sin_theta = std::sqrt(1. - cos_theta * cos_theta);
  if (etai_over_etat * sin_theta > 1.) {
    Vec3 reflected = reflect(unit_direction, rec.normal);
    srec.specular_ray = Ray(rec.p, reflected);
    return true;
  }
  float reflect_prob = schlick(cos_theta, etai_over_etat);
  if (reflect_prob > utils::random_float()) {
    Vec3 reflected = reflect(unit_direction, rec.normal);
    srec.specular_ray = Ray(rec.p, reflected);
    return true;
  }
  Vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
  srec.specular_ray = Ray(rec.p, refracted);
  return true;
}

bool DiffuseLight::scatter(const Ray &r_in, const hit_record &rec,
                           scatter_record &srec) const {
  return false;
}

Color DiffuseLight::emitted(const Ray &r_in, const hit_record &rec, float u,
                            float v, const Point3 &p) const {
  return emit->value(u, v, p);
}

bool isotropic::scatter(const Ray &r_in, const hit_record &rec,
                        scatter_record &srec) const {
  srec.attenuation = tex->value(rec.u, rec.v, rec.p);
  srec.pdf_ptr = std::make_unique<Sphere_pdf>();
  srec.is_specular = false;
  return true;
}

float isotropic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                const Ray &scattered) const {
  return 1.0f / (4.0f * utils::TRACER_PI);
}

} // namespace tracer