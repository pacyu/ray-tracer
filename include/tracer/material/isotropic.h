#pragma once
#include "tracer/core/material.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class Isotropic : public Material {
public:
  Isotropic(const Color &albedo)
      : tex(std::make_shared<texture::SolidColor>(albedo)) {}
  Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Vec3 scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const override;

private:
  std::shared_ptr<Texture> tex;
};

} // namespace material
} // namespace tracer