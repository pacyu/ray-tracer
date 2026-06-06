#include "tracer/material/standard_material.h"

namespace tracer {
namespace material {

bool StandardMaterial::scatter(const Ray &r_in, const hit_record &rec,
                               scatter_record &srec) const {
  // 从贴图或基础颜色获取当前的 Albedo
  Vec3 current_albedo = albedo->value(rec.u, rec.v, rec.p);

  float select_rand = tracer::math::random_float();

  // =================================================================
  // 状态分支一：绝缘体透射/折射 (玻璃)
  // =================================================================
  if (select_rand < transmission) {
    srec.is_specular = true;
    srec.attenuation = current_albedo; // 染色玻璃

    float etai = 1.0f;
    float etat = ior;
    if (!rec.front_face) {
      std::swap(etai, etat);
    }

    Vec3 unit_dir = normalize(r_in.direction());
    float cos_theta = std::min(dot(-unit_dir, rec.normal), 1.0f);

    float fresnel = optics::fresnel_schlick(cos_theta, etai, etat);

    Vec3 outbound_dir;
    // 如果触发全反射或者随机概率小于菲涅尔反射项，时光线发生反射
    if (fresnel > tracer::math::random_float()) {
      outbound_dir = optics::reflect(unit_dir, rec.normal);
    } else {
      outbound_dir = optics::refract(unit_dir, rec.normal, etai / etat);
    }

    // 引入粗糙度扰动 (如果是完美光滑玻璃，roughness 为 0 则不抖动)
    if (roughness > 0.0f) {
      // random_in_unit_sphere 替换为你项目里的球体内随机向量函数
      outbound_dir =
          normalize(outbound_dir + roughness * math::random_in_unit_sphere());
    }

    srec.specular_ray = Ray(rec.p, outbound_dir);
    return true;
  }

  // =================================================================
  // 状态分支二：镜面金属高光 (特指低粗糙度下的高金属性表面)
  // =================================================================
  // 当物体非常光滑且极具金属感时，我们将其视作 Specular 射线处理
  if (select_rand < (transmission + metallic) && roughness < 0.05f) {
    srec.is_specular = true;
    srec.attenuation = current_albedo;

    Vec3 reflected = optics::reflect(normalize(r_in.direction()), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected);
    return true;
  }

  // =================================================================
  // 状态分支三：粗糙微表面 / 漫反射混合 (走 PDF 重要性采样)
  // =================================================================
  // 包含传统塑料、粗糙金属、木材等，进入混和散射模型
  srec.is_specular = false;
  srec.attenuation = current_albedo;

  srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);

  return true;
}

Vec3 StandardMaterial::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                      const Ray &scattered) const {
  // 对于被视作纯镜面反射/折射的分支，不参与 scattering_pdf 的连续评级
  // 只有粗糙表面和朗伯体漫反射需要返回对应的余弦半球 PDF
  float cosine = dot(rec.normal, normalize(scattered.direction()));
  if (cosine < 0.0f) {
    // 如果散射光线跑到了表面下方，对于不透明物体其概率为 0
    return Vec3(0.0f, 0.0f, 0.0f);
  }
  // 标准的余弦半球散射 PDF = cos(theta) / pi
  float pdf_val = cosine / math::TRACER_PI;
  return Vec3(pdf_val, pdf_val, pdf_val);
}

} // namespace material
} // namespace tracer