#pragma once
#include "tracer/core/hittable.h"
#include "tracer/utils/drand48.h"
#include <tuple>

namespace tracer {

// 统一使用归一化坐标计算
inline double h_normalized(double x, double y, double z) {
  double a = x * x + 2.25 * y * y + z * z - 1.0;
  return a * a * a - (x * x + 0.1125 * y * y) * (z * z * z);
}

// 梯度函数：用于牛顿迭代和法线计算
inline Vec3 gradient_normalized(Point3 p_loc) {
  double x = p_loc.x(), y = p_loc.y(), z = p_loc.z();
  double a = x * x + 2.25 * y * y + z * z - 1.0;
  double a2 = a * a;
  double z2 = z * z;
  double z3 = z2 * z;

  double dx = 6.0 * x * a2 - 2.0 * x * z3;
  double dy = 13.5 * y * a2 - 0.225 * y * z3;
  double dz = 6.0 * z * a2 - 3.0 * z2 * (x * x + 0.1125 * y * y);

  return Vec3(static_cast<float>(dx), static_cast<float>(dy),
              static_cast<float>(dz));
}

class Heart : public hittable {
public:
  Heart(Point3 center, float r, std::shared_ptr<Material> m)
      : center(center), rho(r), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &o) const override;

  Point3 center;
  float rho;
  std::shared_ptr<Material> mat_ptr;
};

} // namespace tracer
