#pragma once

#include "tracer/core/aabb.h"
#include "tracer/core/hittable.h"
#include "tracer/core/ray.h"
#include "tracer/utils/drand48.h"
#include "tracer/utils/math.h"

namespace tracer {

class XYRect : public hittable {
public:
  XYRect(float x0, float x1, float y0, float y1, float k,
         std::shared_ptr<Material> m)
      : x0(x0), x1(x1), y0(y0), y1(y1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1, hit_record &rec) const;

  virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
    return true;
  }

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override {
    hit_record rec;
    if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
      return 0;

    auto area = (x1 - x0) * (y1 - y0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  virtual Vec3 random(const Point3 &origin) const override {
    auto random_point =
        Point3(utils::random_float(x0, x1), utils::random_float(y0, y1), k);
    return random_point - origin;
  }

  std::shared_ptr<Material> mat_ptr;
  float x0, x1, y0, y1, k;
};

class XZRect : public hittable {
public:
  XZRect(float x0, float x1, float z0, float z1, float k,
         std::shared_ptr<Material> m)
      : x0(x0), x1(x1), z0(z0), z1(z1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1, hit_record &rec) const;

  virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = AABB(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
    return true;
  }

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override {
    hit_record rec;
    if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
      return 0;

    auto area = (x1 - x0) * (z1 - z0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  virtual Vec3 random(const Point3 &origin) const override {
    auto random_point =
        Point3(utils::random_float(x0, x1), k, utils::random_float(z0, z1));
    return random_point - origin;
  }

  std::shared_ptr<Material> mat_ptr;
  float x0, x1, z0, z1, k;
};

class YZRect : public hittable {
public:
  YZRect(float y0, float y1, float z0, float z1, float k,
         std::shared_ptr<Material> m)
      : y0(y0), y1(y1), z0(z0), z1(z1), k(k), mat_ptr(m) {}

  virtual bool hit(const Ray &r, float t0, float t1, hit_record &rec) const;

  virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = AABB(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
    return true;
  }

  virtual float pdf_value(const Point3 &origin, const Vec3 &v) const override {
    hit_record rec;
    if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
      return 0;

    auto area = (y1 - y0) * (z1 - z0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  virtual Vec3 random(const Point3 &origin) const override {
    auto random_point =
        Point3(k, utils::random_float(y0, y1), utils::random_float(z0, z1));
    return random_point - origin;
  }

  std::shared_ptr<Material> mat_ptr;
  float y0, y1, z0, z1, k;
};

} // namespace tracer