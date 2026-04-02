#pragma once
#include "tracer/core/material.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class Lambertian : public Material {
public:
  Lambertian(const Color &a)
      : albedo(std::make_shared<texture::SolidColor>(a)) {}
  Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Vec3 scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const override;

  std::shared_ptr<Texture> albedo;
};

} // namespace material
} // namespace tracer