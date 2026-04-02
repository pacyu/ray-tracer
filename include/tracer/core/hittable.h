#pragma once
#include "tracer/core/aabb.h"
#include "tracer/core/ray.h"
#include <memory>

namespace tracer {

class Material;

struct hit_record {
  Point3 p;
  Vec3 normal;
  std::shared_ptr<Material> mat_ptr;
  float t;
  float u;
  float v;
  bool front_face;
  float roughness;

  void set_face_normal(const Ray &r, const Vec3 &outward_normal);
};

class hittable {
public:
  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const = 0;

  virtual bool bounding_box(float t0, float t1, AABB &output_box) const = 0;

  virtual std::shared_ptr<Material> get_material() const = 0;

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const { return 0.0; }

  virtual Vec3 random(const Vec3 &o) const { return Vec3(1, 0, 0); }

  virtual void refit(float t0, float t1) {}
};

class flip_face : public hittable {
public:
  flip_face() = default;
  flip_face(std::shared_ptr<hittable> p) : ptr(p) {}

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;

  virtual bool bounding_box(float time0, float time1,
                            AABB &output_box) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return ptr->get_material();
  }

  virtual float pdf_value(const Point3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Vec3 &o) const override;

public:
  std::shared_ptr<hittable> ptr;
};

} // namespace tracer