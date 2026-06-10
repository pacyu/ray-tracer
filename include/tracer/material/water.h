#pragma once
#include "tracer/core/material.h"
#include "tracer/core/pdf.h"
#include "tracer/math/optics.h"

namespace tracer {
namespace material {

class Water : public Material {
public:
  float ior;       // 折射率
  float roughness; // 基础粗糙度

  Water(float ior = 1.333f, float rough = 0.05f) : ior(ior), roughness(rough) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;
};

} // namespace material
} // namespace tracer