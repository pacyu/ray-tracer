#pragma once
#include "tracer/core/aarect.h"
#include "tracer/core/hittable_list.h"

namespace tracer {

class Box : public hittable {
public:
  Box() {}
  Box(const Point3 &p0, const Point3 &p1, std::shared_ptr<Material> mat_ptr);

  virtual bool hit(const Ray &r, float t0, float t1,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &o) const override;

  Point3 box_min;
  Point3 box_max;
  hittable_list sides;
};

} // namespace tracer
