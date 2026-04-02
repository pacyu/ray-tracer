#pragma once
#include "tracer/core/material.h"

namespace tracer {
namespace material {

class Metal : public Material {
public:
  Metal(const Color &a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  Color albedo;
  float fuzz;
};

} // namespace material
} // namespace tracer