#include "tracer/core/constant_medium.h"

namespace tracer {

bool ConstantMedium::hit(const Ray &r, float t_min, float t_max,
                         hit_record &rec) const {
  hit_record rec1, rec2;

  if (!boundary->hit(r, -utils::inf, +utils::inf, rec1))
    return false;

  if (!boundary->hit(r, rec1.t + 0.0001, utils::inf, rec2))
    return false;

  if (rec1.t < t_min)
    rec1.t = t_min;
  if (rec2.t > t_max)
    rec2.t = t_max;

  if (rec1.t >= rec2.t)
    return false;

  if (rec1.t < 0)
    rec1.t = 0;

  auto ray_length = r.direction().length();
  auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
  auto hit_distance = neg_inv_density * std::log(tracer::utils::random_float());

  if (hit_distance > distance_inside_boundary)
    return false;

  rec.t = rec1.t + hit_distance / ray_length;
  rec.p = r.at(rec.t);

  rec.normal = Vec3(1, 0, 0); // arbitrary
  rec.front_face = true;      // also arbitrary
  rec.mat_ptr = phase_function;

  return true;
}

} // namespace tracer