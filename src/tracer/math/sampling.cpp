#include "tracer/math/sampling.h"

namespace tracer {
namespace math {

Vec3 random_cosine_direction() {
  auto r1 = random_float();
  auto r2 = random_float();
  auto z = sqrt(1 - r2);

  auto phi = 2 * tracer::math::TRACER_PI * r1;
  auto x = cos(phi) * sqrt(r2);
  auto y = sin(phi) * sqrt(r2);

  return Vec3(x, y, z);
}

Vec3 random_unit_vector() {
  float a = random_float(0., 2 * tracer::math::TRACER_PI),
        z = random_float(-1., 1.), r = std::sqrt(1 - z * z);
  return Vec3(r * std::cos(a), r * std::sin(a), z);
}

Vec3 random_to_sphere(float radius, float distance_squared) {
  float r1 = random_float();
  float r2 = random_float();
  float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

  float phi = 2 * tracer::math::TRACER_PI * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  auto y = sin(phi) * sqrt(1 - z * z);

  return Vec3(x, y, z);
}

Vec3 random_in_unit_sphere() {
  Vec3 p;
  do {
    p = 2.0f * Vec3(random_float(), random_float(), random_float()) -
        Vec3(1, 1, 1);
  } while (p.squared_length() >= 1.0f);
  return p;
}

Vec3 random_triangle_barycentric() {
  float r1 = std::sqrt(random_float());
  float r2 = random_float();

  // 映射到重心坐标：(1 - sqrt(r1), sqrt(r1) * (1 - r2), sqrt(r1) * r2)
  return Vec3(1.0f - r1, r1 * (1.0f - r2), r1 * r2);
}

Vec3 sample_HG(const Vec3 &wo, float g) {
  float xi1 = random_float();
  float xi2 = random_float();

  float cos_theta;
  if (fabs(g) < 1e-3f) {
    cos_theta = 1 - 2 * xi1;
  } else {
    float sq = (1 - g * g) / (1 - g + 2 * g * xi1);
    cos_theta = (1 + g * g - sq * sq) / (2 * g);
  }

  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  float phi = 2 * tracer::math::TRACER_PI * xi2;

  // 构建局部坐标系
  Vec3 w = unit_vector(wo);
  Vec3 u = unit_vector(fabs(w.x()) > 0.1 ? cross(Vec3(0, 1, 0), w)
                                         : cross(Vec3(1, 0, 0), w));
  Vec3 v = cross(w, u);

  return unit_vector(sin_theta * cos(phi) * u + sin_theta * sin(phi) * v +
                     cos_theta * w);
}

Vec3 sample_GGX(float alpha) {
  float xi1 = random_float();
  float xi2 = random_float();

  float phi = 2.0f * tracer::math::TRACER_PI * xi1;

  // GGX 角度采样公式
  float cos_theta = sqrt((1.0f - xi2) / (1.0f + (alpha * alpha - 1.0f) * xi2));
  float sin_theta = sqrt(std::max(0.0f, 1.0f - cos_theta * cos_theta));

  return Vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
}

} // namespace math
} // namespace tracer