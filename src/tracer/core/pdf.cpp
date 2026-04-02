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

Mixture_pdf::Mixture_pdf(const PDF *p0, const PDF *p1) {
  p[0] = p0;
  p[1] = p1;
}

float Mixture_pdf::value(const Vec3 &direction) const {
  if (!p[0] || !p[1])
    return 0.0f;
  return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
}

Vec3 Mixture_pdf::generate() const {
  if (math::random_float() < 0.5f)
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

} // namespace tracer