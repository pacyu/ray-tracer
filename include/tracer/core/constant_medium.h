#pragma once
#include "tracer/core/hittable.h"
#include "tracer/core/material.h"
#include "tracer/core/texture.h"

namespace tracer {

class ConstantMedium : public hittable {
public:
  ConstantMedium(std::shared_ptr<hittable> boundary, double density,
                 std::shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(std::make_shared<isotropic>(tex)) {}

  ConstantMedium(std::shared_ptr<hittable> boundary, double density,
                 const Color &albedo)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(std::make_shared<isotropic>(albedo)) {}

  bool hit(const Ray &r, float t_min, float t_max,
           hit_record &rec) const override;

  bool bounding_box(float t0, float t1, AABB &output_box) const override {
    return boundary->bounding_box(t0, t1, output_box);
  }

private:
  std::shared_ptr<hittable> boundary;
  double neg_inv_density;
  std::shared_ptr<Material> phase_function;
};

} // namespace tracer
