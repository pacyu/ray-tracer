#pragma once
#include "tracer/core/material.h"

namespace tracer {
namespace material {

class GlossyPlastic : public Material {
public:
  GlossyPlastic(const Color &albedo, float ior) : albedo(albedo), ior(ior) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Vec3 scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const override;
  Color albedo;
  float ior;
};

} // namespace material
} // namespace tracer