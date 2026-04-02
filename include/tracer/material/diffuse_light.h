#pragma once
#include "tracer/core/material.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class DiffuseLight : public Material {
public:
  DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
  DiffuseLight(Color c) : emit(std::make_shared<texture::SolidColor>(c)) {}

  virtual bool is_emitter() const override { return true; }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Color emitted(const Ray &r_in, const hit_record &rec, float u,
                        float v, const Point3 &p) const override;

  std::shared_ptr<Texture> emit;
};

} // namespace material
} // namespace tracer