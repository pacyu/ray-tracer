#pragma once
#include "tracer/core/material.h"
#include "tracer/math/math.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

float charlie_brdf(const Ray &r_in, const Ray &scattered, const Vec3 &normal,
                   float roughness);

class Cloth : public Material {
public:
  Cloth(const Color &a, float roughness)
      : albedo(std::make_shared<texture::SolidColor>(a)), roughness(roughness) {
  }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const scatter_record &srec,
                               const Ray &scattered) const override;

private:
  std::shared_ptr<Texture> albedo;
  float roughness; // 粗糙度
};

} // namespace material
} // namespace tracer
