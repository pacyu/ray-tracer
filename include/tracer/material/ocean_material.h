#pragma once
#include "tracer/core/material.h"
#include "tracer/math/optics.h"
#include "tracer/core/pdf.h"

namespace tracer {
namespace material {

class OceanMaterial : public Material {
public:
  float ior;       // 折射率
  float roughness; // 基础粗糙度

  OceanMaterial(float ior = 1.333f, float rough = 0.05f)
      : ior(ior), roughness(rough) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Vec3 scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const override;
};

} // namespace material
} // namespace tracer