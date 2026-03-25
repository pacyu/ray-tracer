#pragma once
#include "tracer/core/aabb.h"
#include "tracer/core/hittable.h"
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
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Vec3 &o) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return nullptr;
  }

  std::vector<std::shared_ptr<hittable>> objects;
};

} // namespace tracer
