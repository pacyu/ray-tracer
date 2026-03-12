#pragma once

#include "tracer/core/hittable.h"

namespace tracer {

class RotateY : public hittable {
public:
  RotateY(std::shared_ptr<hittable> p, float angle);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const;
  virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = bbox;
    return hasbox;
  }

  std::shared_ptr<hittable> ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  AABB bbox;
};

} // namespace tracer
