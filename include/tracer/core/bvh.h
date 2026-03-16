#pragma once
#include "tracer/core/aabb.h"
#include "tracer/core/hittable.h"
#include "tracer/core/hittable_list.h"

namespace tracer {

class BVH : public hittable {
public:
  BVH(hittable_list list) : BVH(list.objects, 0, list.objects.size()) {}

  BVH(std::vector<std::shared_ptr<hittable>> &objects, size_t start,
      size_t end);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

private:
  std::shared_ptr<hittable> left;
  std::shared_ptr<hittable> right;
  AABB bbox;

  static bool box_compare(const std::shared_ptr<hittable> a,
                          const std::shared_ptr<hittable> b, int axis) {
    AABB box_a, box_b;
    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
      std::cerr << "No bounding box in bvh_node constructor.\n";
    return box_a.min[axis] < box_b.min[axis];
  }

  static bool box_x_compare(const std::shared_ptr<hittable> a,
                            const std::shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const std::shared_ptr<hittable> a,
                            const std::shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const std::shared_ptr<hittable> a,
                            const std::shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
  }
};

} // namespace tracer