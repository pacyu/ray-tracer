#pragma once
#include "tracer/core/aabb.h"
#include "tracer/core/hittable.h"
#include "tracer/core/ray.h"
#include "tracer/math/drand48.h"
#include "tracer/math/math.h"

namespace tracer {
namespace geometry {

class XYRect : public hittable {
public:
  XYRect(float x0, float x1, float y0, float y1, float k,
         std::shared_ptr<Material> m)
      : x0(x0), x1(x1), y0(y0), y1(y1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &origin) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return mat_ptr;
  }

  std::shared_ptr<Material> mat_ptr;
  float x0, x1, y0, y1, k;
};

class XZRect : public hittable {
public:
  XZRect(float x0, float x1, float z0, float z1, float k,
         std::shared_ptr<Material> m)
      : x0(x0), x1(x1), z0(z0), z1(z1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &origin) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return mat_ptr;
  }

  std::shared_ptr<Material> mat_ptr;
  float x0, x1, z0, z1, k;
};

class YZRect : public hittable {
public:
  YZRect(float y0, float y1, float z0, float z1, float k,
         std::shared_ptr<Material> m)
      : y0(y0), y1(y1), z0(z0), z1(z1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override;

  virtual Vec3 random(const Point3 &origin) const override;

  virtual std::shared_ptr<Material> get_material() const override {
    return mat_ptr;
  }

  std::shared_ptr<Material> mat_ptr;
  float y0, y1, z0, z1, k;
};

} // namespace geometry
} // namespace tracer