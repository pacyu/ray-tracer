#include "tracer/material/standard_material.h"

namespace tracer {
namespace material {

bool StandardMaterial::scatter(const Ray &r_in, const hit_record &rec,
                               scatter_record &srec) const {
  // 纹理采样
  Vec3 current_albedo = albedo->value(rec.u, rec.v, rec.p);

  float current_roughness = roughness;
  if (roughness_map) {
    current_roughness = roughness_map->value(rec.u, rec.v, rec.p).x();
  }
  current_roughness = std::clamp(current_roughness, 0.01f, 1.0f);

  float current_metallic = metallic;
  if (metallic_map) {
    Vec3 ks = metallic_map->value(rec.u, rec.v, rec.p);
    current_metallic = (ks.x() + ks.y() + ks.z()) / 3.0f;
    current_metallic = std::clamp(current_metallic, 0.0f, 1.0f);
  }

  float current_alpha = alpha;
  if (alpha_map) {
    current_alpha = alpha_map->value(rec.u, rec.v, rec.p).x();
    current_alpha = std::clamp(current_alpha, 0.0f, 1.0f);
  }

  // 法线贴图
  Vec3 hit_normal = rec.normal;
  if (normal_map) {
    Vec3 tn = normal_map->value(rec.u, rec.v, rec.p);
    tn = 2.0f * tn - Vec3(1.0f, 1.0f, 1.0f);
    tn = normalize(tn);
    hit_normal = normalize(tn.x() * rec.tangent + tn.y() * rec.bitangent +
                           tn.z() * rec.normal);
  }

  bool new_front_face = dot(r_in.direction(), hit_normal) < 0.0f;
  if (double_sided && !new_front_face) {
    hit_normal = -hit_normal;
    new_front_face = !new_front_face;
  }

  float rand_val = math::random_float();

  // 透明/折射分支
  if (rand_val < current_alpha && current_alpha < 1.0f) {
    srec.is_specular = true;
    srec.attenuation = current_albedo;

    float etai = 1.0f, etat = ior;
    if (!new_front_face)
      std::swap(etai, etat);

    Vec3 unit_dir = normalize(r_in.direction());
    float cos_theta = std::min(dot(-unit_dir, hit_normal), 1.0f);
    float fresnel = optics::fresnel_schlick(cos_theta, etai, etat);

    Vec3 out_dir;
    if (fresnel > math::random_float()) {
      out_dir = optics::reflect(unit_dir, hit_normal);
    } else {
      out_dir = optics::refract(unit_dir, hit_normal, etai / etat);
    }

    if (current_roughness > 0.0f) {
      out_dir = normalize(out_dir +
                          current_roughness * math::random_in_unit_sphere());
    }
    srec.specular_ray = Ray(rec.p, out_dir);
    return true;
  }

  // 完美镜面金属分支
  if (current_metallic > 0.95f && current_roughness < 0.05f) {
    srec.is_specular = true;
    srec.attenuation = current_albedo;
    Vec3 reflected = optics::reflect(normalize(r_in.direction()), hit_normal);
    srec.specular_ray = Ray(rec.p, reflected);
    return true;
  }

  // 漫反射 + 粗糙金属（混合 PDF）
  srec.is_specular = false;
  srec.attenuation = current_albedo;

  Vec3 view_dir = normalize(-r_in.direction());
  auto ggx = std::make_shared<GGX_pdf>(hit_normal, view_dir, current_roughness);
  auto cosine = std::make_shared<Cosine_pdf>(hit_normal);
  float blend = std::clamp(current_metallic, 0.0f, 1.0f);
  srec.pdf_ptr = std::make_shared<Mixture_pdf>(ggx, cosine, blend);

  return true;
}

float StandardMaterial::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                       const scatter_record &srec,
                                       const Ray &scattered) const {
  if (srec.is_specular)
    return 0.0f;
  if (srec.pdf_ptr) {
    return srec.pdf_ptr->value(scattered.direction());
  }
  return 0.0f;
}

} // namespace material
} // namespace tracer