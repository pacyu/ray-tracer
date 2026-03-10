#pragma once
#include "aabb.h"
#include "hittable.h"
#include "tracer/utils/drand48.h"
#include <memory>
#include <vector>

namespace tracer {

class hittable_list : public hittable {
public:
  hittable_list() {}
  hittable_list(std::shared_ptr<hittable> object) : objects{object} {}

  void clear() { objects.clear(); }
  void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const;

  virtual bool bounding_box(float t0, float t1, AABB &output_box) const;

  float pdf_value(const Point3 &o, const Vec3 &v) const {
    auto weight = 1.0 / objects.size();
    auto sum = 0.0;

    for (const auto &object : objects)
      sum += weight * object->pdf_value(o, v);

    return sum;
  }

  Vec3 random(const Vec3 &o) const {
    auto int_size = static_cast<int>(objects.size());
    return objects[utils::random_int(0, int_size - 1)]->random(o);
  }

  std::vector<std::shared_ptr<hittable>> objects;
};

} // namespace tracer
