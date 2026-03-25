#pragma once
#include "tracer/core/hittable.h"
#include "tracer/core/onb.h"
#include "tracer/core/ray.h"
#include "tracer/utils/drand48.h"

namespace tracer {

void get_sphere_uv(const Point3 &p, float &u, float &v);

class Sphere : public hittable {
public:
  Sphere(Point3 center, float radius, std::shared_ptr<Material> m)
      : center(center), radius(radius), mat_ptr(m) {}

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
  float radius;
  std::shared_ptr<Material> mat_ptr;
};

} // namespace tracer
