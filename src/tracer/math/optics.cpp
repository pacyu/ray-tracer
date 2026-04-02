#include "tracer/math/optics.h"

namespace tracer {
namespace optics {

Vec3 reflect(const Vec3 &v, const Vec3 &n) { return v - 2 * dot(v, n) * n; }

Vec3 refract(const Vec3 &uv, const Vec3 &n, float etai_over_etat) {
  float cos_theta = dot(-uv, n);
  Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3 r_out_parallel =
      -std::sqrt(std::abs(1.f - r_out_perp.squared_length())) * n;
  return r_out_perp + r_out_parallel;
}

bool refract(const Vec3 &v, const Vec3 &n, float ni_over_nt, Vec3 &refracted) {
  Vec3 uv = unit_vector(v);
  float dt = dot(uv, n);

  // 判别式，用于检查是否发生全反射
  float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);

  if (discriminant > 0) {
    // 斯涅尔定律的矢量形式推导
    refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
    refracted = unit_vector(refracted); // 确保输出是单位向量
    return true;
  } else {
    return false; // 发生全反射
  }
}

float schlick(float cosine, float ref_idx) {
  float r0 = (1.f - ref_idx) / (1.f + ref_idx);
  r0 *= r0;
  return r0 + (1.f - r0) * std::pow((1.f - cosine), 5.f);
}

float D_GGX(float NoH, float alpha) {
  float a2 = alpha * alpha;
  float denom = NoH * NoH * (a2 - 1.0f) + 1.0f;
  return a2 / (tracer::math::TRACER_PI * denom * denom);
}

float G_Smith(float NoV, float NoL, float alpha) {
  float k = alpha * 0.5f;
  float g1 = NoV / (NoV * (1 - k) + k);
  float g2 = NoL / (NoL * (1 - k) + k);
  return g1 * g2;
}

float fresnel_schlick(float cos_theta, float etai, float etat) {
  float r0 = (etai - etat) / (etai + etat);
  r0 = r0 * r0;

  // Schlick 近似
  return r0 + (1.0f - r0) * std::pow(1.0f - cos_theta, 5.0f);
}

} // namespace optics
} // namespace tracer