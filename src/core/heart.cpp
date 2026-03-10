#include "tracer/core/heart.h"

namespace tracer {

bool Heart::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  Vec3 oc = r.origin() - center;
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - (rho * 1.5f) * (rho * 1.5f);

  float t = t_min;
  float disc = b * b - c;
  if (disc < 0)
    return false;

  float t_entry = -b - sqrt(disc);
  float t_exit = -b + sqrt(disc);
  t = std::max(t_min, t_entry - 2.0f);
  float t_end = std::min(t_max, t_exit + 2.0f);

  float base_step = 0.005f * rho;

  for (int i = 0; i < 800 && t < t_end; i++) {
    Point3 p_loc = (r.at(t) - center) / rho;
    double val = h_normalized(p_loc.x(), p_loc.y(), p_loc.z());

    if (val < 0) {
      // 发现交点，牛顿迭代精修
      float t_fine = t;
      for (int j = 0; j < 3; j++) {
        Point3 p_it = (r.at(t_fine) - center) / rho;
        double f = h_normalized(p_it.x(), p_it.y(), p_it.z());
        Vec3 grad = gradient_normalized(p_it);
        float f_prime = dot(grad / rho, r.direction());
        if (std::abs(f_prime) > 1e-7) {
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
      rec.p += rec.normal * 0.01f;
      rec.mat_ptr = mat_ptr;
      return true;
    }

    float adaptive_factor = 1.0f + std::abs(static_cast<float>(val)) * 10.0f;
    float current_step = base_step * adaptive_factor;

    // 限制最大单步跨度，防止直接“穿透”整个心形
    t += std::min(current_step, 0.2f * rho);
  }
  return false;
}

bool Heart::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(center - Vec3(rho * 1.5f, rho * 1.5f, rho * 1.5f),
                    center + Vec3(rho * 1.5f, rho * 1.5f, rho * 1.5f));
  return true;
}

} // namespace tracer