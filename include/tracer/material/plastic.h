#pragma once
#include "tracer/core/material.h"

namespace tracer {
namespace material {

class Plastic : public Material {
public:
  Plastic(const Color &albedo, float roughness, float ior)
      : albedo(albedo), roughness(roughness), ior(ior) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Color scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const override;

private:
  Color albedo;    // 底部颜色
  float roughness; // 表面涂层的粗糙度
  float ior;       // 涂层的折射率（通常塑料约为 1.5）
};

} // namespace material
} // namespace tracer