#pragma once
#include "tracer/core/material.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class Plastic : public Material {
public:
  Plastic(const Color &albedo, float roughness, float ior)
      : albedo(std::make_shared<texture::SolidColor>(albedo)), roughness(roughness), ior(ior) {}

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const scatter_record &srec,
                               const Ray &scattered) const override;

private:
  std::shared_ptr<Texture> albedo;    // 底部颜色
  float roughness; // 表面涂层的粗糙度
  float ior;       // 涂层的折射率（通常塑料约为 1.5）
};

} // namespace material
} // namespace tracer