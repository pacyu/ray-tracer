#pragma once
#include "tracer/core/hittable.h"
#include "tracer/math/math.h"

namespace tracer {
namespace transform {

class RotateX : public hittable {
public:
  RotateX(std::shared_ptr<hittable> p, float angle);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override {
    output_box = bbox;
    return hasbox;
  }
  virtual std::shared_ptr<Material> get_material() const override {
    return ptr->get_material();
  }

  std::shared_ptr<hittable> ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  AABB bbox;
};

class RotateY : public hittable {
public:
  RotateY(std::shared_ptr<hittable> p, float angle);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override {
    output_box = bbox;
    return hasbox;
  }
  virtual std::shared_ptr<Material> get_material() const override {
    return ptr->get_material();
  }

  std::shared_ptr<hittable> ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  AABB bbox;
};

class RotateZ : public hittable {
public:
  RotateZ(std::shared_ptr<hittable> p, float angle);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override {
    output_box = bbox;
    return hasbox;
  }
  virtual std::shared_ptr<Material> get_material() const override {
    return ptr->get_material();
  }

  std::shared_ptr<hittable> ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  AABB bbox;
};

} // namespace geometry
} // namespace tracer
