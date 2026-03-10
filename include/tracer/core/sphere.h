#pragma once
#include "hittable.h"
#include "onb.h"
#include "ray.h"
#include "tracer/utils/math.h"

namespace tracer {

inline Vec3 random_to_sphere(float radius, float distance_squared) {
  auto r1 = utils::random_float();
  auto r2 = utils::random_float();
  auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

  auto phi = 2 * utils::M_PI * r1;
  auto x = cos(phi) * sqrt(1 - z * z);
  auto y = sin(phi) * sqrt(1 - z * z);

  return Vec3(x, y, z);
}

inline void get_sphere_uv(const Point3 &p, float &u, float &v) {
  auto phi = atan2(p.z(), p.x());
  auto theta = asin(p.y());
  u = 1 - (phi + utils::M_PI) / (2 * utils::M_PI);
  v = (theta + utils::M_PI / 2) / utils::M_PI;
}

class Sphere : public hittable {
public:
  Sphere(Point3 center, float radius, std::shared_ptr<Material> m)
      : center(center), radius(radius), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const;
  virtual bool bounding_box(float t0, float t1, AABB &output_box) const;

  float pdf_value(const Point3 &o, const Vec3 &v) const {
    hit_record rec;
    if (!this->hit(Ray(o, v), 0.001, utils::inf, rec))
      return 0;

    auto cos_theta_max =
        sqrt(1 - radius * radius / (center - o).squared_length());
    auto solid_angle = 2 * utils::M_PI * (1 - cos_theta_max);

    return 1 / solid_angle;
  }

  Vec3 random(const Point3 &o) const {
    Vec3 direction = center - o;
    auto distance_squared = direction.squared_length();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
  }

  Point3 center;
  float radius;
  std::shared_ptr<Material> mat_ptr;
};

} // namespace tracer
