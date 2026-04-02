#include "tracer/material/plastic.h"

namespace tracer {
namespace material {

bool Plastic::scatter(const Ray &r_in, const hit_record &rec,
                      scatter_record &srec) const {
  // 1. 计算菲涅尔系数 (Schlick近似)
  float cos_theta =
      std::min(dot(-unit_vector(r_in.direction()), rec.normal), 1.0f);
  float F0 = pow((1.0f - ior) / (1.0f + ior), 2.0f);
  float fresnel = F0 + (1.0f - F0) * pow(1.0f - cos_theta, 5.0f);

  // 2. 决定这根光线是发生“涂层反射”还是“进入基底散射”
  if (tracer::math::random_float() < fresnel) {
    // 镜面/涂层反射 (Specular/Coat)
    srec.is_specular = true;
    Vec3 reflected =
        tracer::optics::reflect(unit_vector(r_in.direction()), rec.normal);
    // 如果 roughness > 0，给反射方向加一点扰动
    srec.specular_ray = Ray(
        rec.p, reflected + roughness * tracer::math::random_in_unit_sphere());
    srec.attenuation = Color(1, 1, 1); // 涂层反射通常不带色
  } else {
    // 基底漫反射 (Diffuse/Base)
    srec.is_specular = false;
    srec.attenuation = albedo;
    srec.pdf_ptr = std::make_unique<Cosine_pdf>(rec.normal);
  }
  return true;
}

Color Plastic::scattering_pdf(const Ray &r_in, const hit_record &rec,
                              const Ray &scattered) const {
  Vec3 N = rec.normal;
  Vec3 V = unit_vector(-r_in.direction());
  Vec3 L = unit_vector(scattered.direction());
  Vec3 H = unit_vector(V + L); // 半程向量

  float NoL = std::max(dot(N, L), 0.0f);
  float NoV = std::max(dot(N, V), 0.0f);
  float NoH = std::max(dot(N, H), 0.0f);
  float HoV = std::max(dot(H, V), 0.0f);

  if (NoL <= 0.0f || NoV <= 0.0f)
    return Color(0, 0, 0);

  // 1. 计算菲涅尔
  // 注意：计算反射能量分配时，通常使用入射角与半程向量的夹角 HoV
  float F = tracer::optics::fresnel_schlick(HoV, 1.0f, ior);

  // 2. 漫反射分量：(1 - F) 的能量才能进入基底
  Color diffuse = (1.0f - F) * albedo * (NoL / tracer::math::TRACER_PI);

  // 3. 镜面分量 (GGX)
  float D = tracer::optics::D_GGX(
      NoH, roughness); // 这里的 roughness 建议是用户输入的平方
  float G = tracer::optics::G_Smith(NoV, NoL, roughness);

  // 标准 Microfacet Specular 公式: (D * G * F) / (4 * NoV * NoL)
  // 因为返回的是 BRDF * cos(theta)，所以最后要乘个 NoL
  float specular_brdf = (D * G) / (4.0f * NoV * NoL + 1e-4f);
  Color specular = F * Color(1, 1, 1) * specular_brdf * NoL;

  return diffuse + specular;
}

} // namespace material
} // namespace tracer