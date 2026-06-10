#include "tracer/core/pdf.h"

namespace tracer {

Cosine_pdf::Cosine_pdf(const Vec3 &w) { uvw.build_from_w(w); }

float Cosine_pdf::value(const Vec3 &direction) const {
  auto cosine = dot(unit_vector(direction), uvw.w());
  return (cosine <= 0) ? 0 : cosine / math::TRACER_PI;
}

Vec3 Cosine_pdf::generate() const {
  return uvw.local(math::random_cosine_direction());
}

Mixture_pdf::Mixture_pdf(const PDF *p0, const PDF *p1, float blend)
    : p{p0, p1}, blend(std::clamp(blend, 0.0f, 1.0f)) {}

float Mixture_pdf::value(const Vec3 &direction) const {
  if (!p[0] || !p[1])
    return 0.0f;
  return blend * p[0]->value(direction) +
         (1.0f - blend) * p[1]->value(direction);
}

Vec3 Mixture_pdf::generate() const {
  if (math::random_float() < blend)
    return p[0]->generate();
  else
    return p[1]->generate();
}

Hittable_pdf::Hittable_pdf(const hittable &p, const Point3 &origin)
    : ptr(p), o(origin) {}

float Hittable_pdf::value(const Vec3 &direction) const {
  return ptr.pdf_value(o, direction);
}

Vec3 Hittable_pdf::generate() const { return ptr.random(o); }

float Sphere_pdf::value(const Vec3 &direction) const {
  return 1 / (4 * tracer::math::TRACER_PI);
}

Vec3 Sphere_pdf::generate() const { return math::random_unit_vector(); }

float GGX_pdf::value(const Vec3 &direction) const {
  Vec3 l = unit_vector(direction);
  Vec3 h = unit_vector(v + l);

  float NoH = dot(n, h);
  float VoH = dot(v, h);
  float NoL = dot(n, l);
  float NoV = dot(n, v);

  if (NoL <= 0 || NoV <= 0 || NoH <= 0 || VoH <= 0)
    return 0.0f;

  // 计算 D 项
  float a2 = alpha * alpha;
  float cos2theta = NoH * NoH;
  float denom = (cos2theta * (a2 - 1.0f) + 1.0f);

  if (denom < 1e-6f)
    return 0.0f;
  float D = a2 / (math::TRACER_PI * denom * denom);

  // 将微表面法线 D(h) 转换为入射光方向的 PDF: pdf(l) = D(h) * NoH / (4 * VoH)
  return (D * NoH) / (4.0f * VoH);
}

Vec3 GGX_pdf::generate() const {
  float r1 = math::random_float();
  float r2 = math::random_float();

  // 基于 alpha 采样 theta
  float phi = 2.0f * math::TRACER_PI * r1;
  float cos_theta = sqrt((1.0f - r2) / (1.0f + (alpha * alpha - 1.0f) * r2));
  float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

  // 得到局部坐标系下的微表面法线 h
  Vec3 h_local(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);

  // 转到世界坐标系
  onb uvw;
  uvw.build_from_w(n);
  Vec3 h = uvw.local(h_local);

  // 根据视线 v 和微表面法线 h 得到反射方向 l
  return tracer::optics::reflect(-v, h);
}

Charlie_pdf::Charlie_pdf(const Vec3 &normal, const Vec3 &view, float roughness,
                         const Vec3 &tangent, const Vec3 &bitangent)
    : n(normal), v(view), alpha(roughness), T(tangent), B(bitangent) {
  // 确保粗糙度有效
  alpha = std::max(alpha, 0.01f);
}

Vec3 Charlie_pdf::generate() const {
  // 1. 采样半程向量 h 的分布（Charlie 各向同性分布）
  //    PDF of h: p_h(h) = (2 + 1/alpha) * (sinθ_h)^(1/alpha) / (2π)
  //    转换为 θ_h 和 φ 采样：
  float phi = 2.0f * math::TRACER_PI * math::random_float();
  float sin_phi = std::sin(phi);
  float cos_phi = std::cos(phi);

  // 采样 sinθ_h: 令 u = sinθ_h^(1/alpha + 1), 则 sinθ_h = u^(alpha/(alpha+1))
  float inv_alpha = 1.0f / alpha;
  float exponent = 1.0f / (inv_alpha + 1.0f); // = alpha/(alpha+1)
  float sin_theta = std::pow(math::random_float(), exponent);
  float cos_theta = std::sqrt(1.0f - sin_theta * sin_theta);

  // 局部坐标系中的半程向量 h_local
  Vec3 h_local(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);

  // 2. 将 h_local 转换到世界空间 (T, B, N)
  Vec3 h_world = h_local.x() * T + h_local.y() * B + h_local.z() * n;
  h_world = normalize(h_world);

  // 3. 根据微表面理论: 反射方向 l = 2*(v·h)h - v
  float v_dot_h = dot(v, h_world);
  if (v_dot_h <= 0.0f) {
    // 视线与半程向量背向，退化处理（返回镜面反射方向）
    return normalize(2.0f * dot(v, n) * n - v);
  }
  Vec3 l_world = 2.0f * v_dot_h * h_world - v;
  if (dot(l_world, n) <= 0.0f) {
    // 若方向朝下，对称翻转到半球上方
    l_world = -l_world;
  }
  return normalize(l_world);
}

float Charlie_pdf::value(const Vec3 &direction) const {
  // 方向必须指向外半球
  if (dot(direction, n) <= 0.0f)
    return 0.0f;

  // 根据微表面映射关系：给定 l，求出半程向量 h = normalize(v + l)
  Vec3 l = normalize(direction);
  Vec3 h = normalize(v + l);
  float v_dot_h = dot(v, h);
  if (v_dot_h <= 0.0f)
    return 0.0f;

  // 计算 h 的局部坐标 (相对于 N)
  Vec3 h_local(dot(h, T), dot(h, B), dot(h, n));
  float sin_theta_h_sq = h_local.x() * h_local.x() + h_local.y() * h_local.y();
  float sin_theta_h = std::sqrt(sin_theta_h_sq);
  float cos_theta_h = std::abs(h_local.z());

  if (sin_theta_h < 1e-6f)
    sin_theta_h = 1e-6f;

  // 计算 p_h(h) = (2 + 1/alpha) * (sinθ_h)^(1/alpha) / (2π)
  float inv_alpha = 1.0f / alpha;
  float p_h = (2.0f + inv_alpha) * std::pow(sin_theta_h, inv_alpha) /
              (2.0f * math::TRACER_PI);

  // 变换雅可比: p(l) = p(h) / (4 * |v·h|)
  float p_l = p_h / (4.0f * v_dot_h);
  if (p_l < 1e-6f)
    p_l = 1e-6f;
  return p_l;
}

} // namespace tracer