#pragma once
#include "tracer/core/material.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class GlossyPlastic : public Material {
public:
  GlossyPlastic(const Color &albedo, float ior)
      : albedo(std::make_shared<texture::SolidColor>(albedo)), ior(ior) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const scatter_record &srec,
                               const Ray &scattered) const override;
  std::shared_ptr<Texture> albedo;
  float ior;
};

} // namespace material
} // namespace tracer