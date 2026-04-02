#pragma once
#include "tracer/core/material.h"

namespace tracer {
namespace material {

class Dielectric : public Material {
public:
  Dielectric(const Color &albedo, float density, float refraction_index)
      : albedo(albedo), density(density), ref_idx(refraction_index) {}

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  Color albedo;
  float density;
  float ref_idx;
};

} // namespace material
} // namespace tracer