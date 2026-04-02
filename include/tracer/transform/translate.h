#pragma once
#include "tracer/core/hittable.h"

namespace tracer {
namespace transform {

class Translate : public hittable {
public:
  Translate(std::shared_ptr<hittable> p, const Vec3 &displacement)
      : ptr(p), offset(displacement) {}

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return ptr->get_material();
  }

  std::shared_ptr<hittable> ptr;
  Vec3 offset;
};

} // namespace transform
} // namespace tracer