#include "tracer/core/hittable_list.h"

namespace tracer {

bool hittable_list::hit(const Ray &r, float t_min, float t_max,
                        hit_record &rec) const {
  hit_record t_rec;
  bool hit_anything = false;
  float closest_so_far = t_max;

  for (const auto &object : objects) {
    if (object->hit(r, t_min, closest_so_far, t_rec)) {
      hit_anything = true;
      closest_so_far = t_rec.t;
      rec = t_rec;
    }
  }
  return hit_anything;
}

bool hittable_list::bounding_box(float t0, float t1, AABB &output_box) const {
  if (objects.empty())
    return false;
  AABB temp_box;
  bool first_box = true;

  for (const auto &object : objects) {
    if (!object->bounding_box(t0, t1, temp_box))
      return false;
    output_box =
        first_box ? temp_box : AABB::surrounding_box(output_box, temp_box);
    first_box = false;
  }
  return true;
}

} // namespace tracer