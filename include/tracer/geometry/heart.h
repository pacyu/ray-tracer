#pragma once
#include "tracer/core/hittable.h"
#include "tracer/math/sampling.h"
#include <tuple>

namespace tracer {
namespace geometry {

// 统一使用归一化坐标计算
float h_normalized(float x, float y, float z);

// 梯度函数：用于牛顿迭代和法线计算
Vec3 gradient_normalized(Point3 p_loc);

class Heart : public hittable {
public:
  Heart(Point3 center, float r, std::shared_ptr<Material> m)
      : center(center), rho(r), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;
  virtual std::shared_ptr<Material> get_material() const override {
    return mat_ptr;
  }

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &o) const override;

  Point3 center;
  float rho;
  std::shared_ptr<Material> mat_ptr;
};

} // namespace geometry
} // namespace tracer
