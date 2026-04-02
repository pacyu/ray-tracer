#pragma once
#include "tracer/core/material.h"
#include "tracer/math/math.h"

namespace tracer {
namespace material {

float charlie_brdf(const Ray &r_in, const Ray &scattered, const Vec3 &normal,
                   float roughness);

class Cloth : public Material {
public:
  Cloth(const Color &a, float roughness) : albedo(a), roughness(roughness) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Color scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const override;

private:
  Color albedo;
  float roughness; // 粗糙度
};

} // namespace material
} // namespace tracer
