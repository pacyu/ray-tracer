#pragma once
#include "tracer/core/material.h"
#include "tracer/math/drand48.h"
#include "tracer/math/optics.h"
#include "tracer/texture/solid_color.h"

namespace tracer {
namespace material {

class StandardMaterial : public Material {
public:
  StandardMaterial() = default;

  StandardMaterial(const Color &a, float m, float r)
      : albedo(std::make_shared<texture::SolidColor>(a)), metallic(m),
        roughness(r) {}

  StandardMaterial(std::shared_ptr<Texture> a, float m, float r)
      : albedo(std::move(a)), metallic(m), roughness(r) {}

  virtual Vec3 emitted(const Ray &r_in, const hit_record &rec, float u, float v,
                       const Vec3 &p) const override {
    return emissive;
  }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Vec3 scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const override;

  // 基础漫反射属性 (对应 mtl 的 Kd / map_Kd)
  std::shared_ptr<Texture> albedo;

  // 自发光属性 (对应 mtl 的 Ke / emissive)
  Vec3 emissive{0.0f, 0.0f, 0.0f};

  // 物理表面微表面属性 (从 mtl 的 Ks 和 Ns 经验换算)
  float metallic{0.0f};
  float roughness{0.5f};

  // 透明/透射属性 (对应 mtl 的 d / Tr / Ni)
  float transmission{0.0f};
  float ior{1.5f}; // 折射率
};

} // namespace material
} // namespace tracer