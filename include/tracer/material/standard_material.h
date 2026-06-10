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
      : albedo(std::make_shared<texture::SolidColor>(a)),
        emissive_map(
            std::make_shared<texture::SolidColor>(Color(0.0f, 0.0f, 0.0f))),
        metallic(m), roughness(r) {}

  StandardMaterial(std::shared_ptr<Texture> a, float m, float r)
      : albedo(std::move(a)),
        emissive_map(
            std::make_shared<texture::SolidColor>(Color(0.0f, 0.0f, 0.0f))),
        metallic(m), roughness(r) {}

  virtual Vec3 emitted(const Ray &r_in, const hit_record &rec, float u, float v,
                       const Vec3 &p) const override {
    return emissive_map->value(rec.u, rec.v, rec.p);
  }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const scatter_record &srec,
                               const Ray &scattered) const override;

  // 基础漫反射属性 (对应 mtl 的 Kd / map_Kd)
  std::shared_ptr<Texture> albedo;

  // Ke / map_Ke 自发光属性
  std::shared_ptr<Texture> emissive_map;

  // 物理表面微表面属性 (从 mtl 的 Ks 和 Ns 经验换算)
  float metallic{0.0f};
  float roughness{0.5f};

  // 非金属镜面反射基准 (F0)，对应 Ks 亮度/颜色
  float specular{0.04f};

  // Tf – 透射滤镜颜色
  Vec3 tf_color{1.0f, 1.0f, 1.0f};

  // Sharpness – 反射清晰度
  float sharpness{60.0f};

  // 纹理
  std::shared_ptr<Texture> ambient_occlusion; // map_Ka / Ka 灰度或颜色
  std::shared_ptr<Texture> metallic_map;      // map_Ks 转金属度
  std::shared_ptr<Texture> roughness_map;     // map_Ns 转粗糙度
  std::shared_ptr<Texture> normal_map;        // map_bump / bump
  std::shared_ptr<Texture> alpha_map;         // map_d / map_Tr
  std::shared_ptr<Texture> reflection_map;    // refl
  std::shared_ptr<Texture> displacement_map;  // disp
  std::shared_ptr<Texture> decal_map;         // decal

  // 光照模型
  int illum_model{2};

  // 透明/透射属性 (对应 mtl 的 d / Tr / Ni)
  float alpha{1.0f};
  float ior{1.5f}; // 折射率 Ni
  bool double_sided = false;
};

} // namespace material
} // namespace tracer