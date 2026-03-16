#include "tracer/core/heart.h"

namespace tracer {

bool Heart::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  Vec3 oc = r.origin() - center;
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - (rho * 1.5f) * (rho * 1.5f);

  float disc = b * b - c;
  if (disc < 0)
    return false;

  float t_entry = -b - sqrt(disc);
  float t_exit = -b + sqrt(disc);
  float t = std::max(t_min, t_entry);
  float t_end = std::min(t_max, t_exit);

  float base_step = 0.005f * rho;

#pragma loop(hint_parallel(0))
  for (int i = 0; i < 256 && t < t_end; i++) {
    Point3 p_loc = (r.at(t) - center) / rho;
    double val = h_normalized(p_loc.x(), p_loc.y(), p_loc.z());

    if (val < 0) {
      // 发现交点，牛顿迭代精修
      float t_fine = t;
      for (int j = 0; j < 16; j++) {
        Point3 p_it = (r.at(t_fine) - center) / rho;
        double f = h_normalized(p_it.x(), p_it.y(), p_it.z());
        Vec3 grad = gradient_normalized(p_it);
        float f_prime = dot(grad / rho, r.direction());
        if (std::abs(f_prime) > 1e-12) {
          t_fine -= utils::clamp(static_cast<float>(f / f_prime), -base_step,
                                 base_step);
        }
      }

      if (t_fine < t_min || t_fine > t_max)
        return false;

      rec.t = t_fine;
      rec.p = r.at(t_fine);
      Vec3 normal = gradient_normalized((rec.p - center) / rho);
      rec.set_face_normal(r, unit_vector(normal));
      rec.p += rec.normal * 0.001f;
      rec.mat_ptr = mat_ptr;
      return true;
    }

    // float adaptive_factor = 1.0f + std::abs(static_cast<float>(val)) * 10.0f;
    // float current_step = base_step * adaptive_factor;

    // // 限制最大单步跨度，防止直接“穿透”整个心形
    // t += std::min(current_step, 0.2f * rho);

    Vec3 grad = gradient_normalized(p_loc);
    float grad_len = grad.length();

    // 使用方向导数或梯度模长来估算安全步长
    // 原理：步长 = 当前函数值 / 变化率（梯度），0.5 为安全系数防止越界
    float adaptive_step =
        static_cast<float>(std::abs(val) / (grad_len + 1e-6f));
    float factor = (val < 0.05) ? 0.2f : 0.5f;
    t += utils::clamp(adaptive_step * factor, base_step, 0.2f * rho);
  }
  return false;
}

bool Heart::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(center - Vec3(rho * 1.5f, rho * 1.5f, rho * 1.5f),
                    center + Vec3(rho * 1.5f, rho * 1.5f, rho * 1.5f));
  return true;
}

float Heart::pdf_value(const Point3 &o, const Vec3 &v) const {
  hit_record rec;

  if (!this->hit(Ray(o, v), 0.001, tracer::utils::inf, rec))
    return 0;

  // 1. 计算心形的近似包围球（基于中心和 rho）
  // 心形大约占据半径为 1.5 * rho 的空间
  float radius = rho * 1.5f;
  auto cos_theta_max =
      sqrt(1 - radius * radius / (center - o).squared_length());
  auto solid_angle = 2 * tracer::utils::TRACER_PI * (1 - cos_theta_max);

  return 1 / solid_angle;
}

Vec3 Heart::random(const Point3 &o) const {
  Vec3 direction = center - o;
  auto distance_squared = direction.squared_length();

  // 构建正交基用于生成球面上随机点
  // 这里简化处理：直接在心形的“包围球”内采样
  float radius = rho * 1.5f;

  return random_to_sphere(radius, distance_squared);
}

} // namespace tracer