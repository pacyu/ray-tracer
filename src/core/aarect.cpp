#include "tracer/core/aarect.h"

namespace tracer {

bool XYRect::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
  return true;
}

float XYRect::pdf_value(const Point3 &origin, const Vec3 &v) const {
  hit_record rec;
  if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
    return 0;

  auto area = (x1 - x0) * (y1 - y0);
  auto distance_squared = rec.t * rec.t * v.squared_length();
  auto cosine = fabs(dot(v, rec.normal) / v.length());

  return distance_squared / (cosine * area);
}

Vec3 XYRect::random(const Point3 &origin) const {
  auto random_point =
      Point3(utils::random_float(x0, x1), utils::random_float(y0, y1), k);
  return random_point - origin;
}

bool XYRect::hit(const Ray &r, float t0, float t1, hit_record &rec) const {
  float t = (k - r.origin().z()) / r.direction().z();
  if (t < t0 || t > t1)
    return false;
  float x = r.origin().x() + t * r.direction().x();
  float y = r.origin().y() + t * r.direction().y();
  if (x < x0 || x > x1 || y < y0 || y > y1)
    return false;
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (y - y0) / (y1 - y0);
  rec.t = t;
  rec.set_face_normal(r, Vec3(0, 0, 1));
  rec.mat_ptr = mat_ptr;
  rec.p = r.at(t);
  return true;
}

bool XZRect::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
  return true;
}

float XZRect::pdf_value(const Point3 &origin, const Vec3 &v) const {
  hit_record rec;
  if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
    return 0;

  auto area = (x1 - x0) * (z1 - z0);
  auto distance_squared = rec.t * rec.t * v.squared_length();
  auto cosine = fabs(dot(v, rec.normal) / v.length());

  return distance_squared / (cosine * area);
}

Vec3 XZRect::random(const Point3 &origin) const {
  auto random_point =
      Point3(utils::random_float(x0, x1), k, utils::random_float(z0, z1));
  return random_point - origin;
}

bool XZRect::hit(const Ray &r, float t0, float t1, hit_record &rec) const {
  float t = (k - r.origin().y()) / r.direction().y();
  if (t < t0 || t > t1)
    return false;
  float x = r.origin().x() + t * r.direction().x();
  float z = r.origin().z() + t * r.direction().z();
  if (x < x0 || x > x1 || z < z0 || z > z1)
    return false;
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (z - z0) / (z1 - z0);
  rec.t = t;
  rec.set_face_normal(r, Vec3(0, 1, 0));
  rec.mat_ptr = mat_ptr;
  rec.p = r.at(t);
  return true;
}

bool YZRect::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
  return true;
}

float YZRect::pdf_value(const Point3 &origin, const Vec3 &v) const {
  hit_record rec;
  if (!this->hit(Ray(origin, v), 0.001, utils::inf, rec))
    return 0;

  auto area = (y1 - y0) * (z1 - z0);
  auto distance_squared = rec.t * rec.t * v.squared_length();
  auto cosine = fabs(dot(v, rec.normal) / v.length());

  return distance_squared / (cosine * area);
}

Vec3 YZRect::random(const Point3 &origin) const {
  auto random_point =
      Point3(k, utils::random_float(y0, y1), utils::random_float(z0, z1));
  return random_point - origin;
}

bool YZRect::hit(const Ray &r, float t0, float t1, hit_record &rec) const {
  float t = (k - r.origin().x()) / r.direction().x();
  if (t < t0 || t > t1)
    return false;
  float y = r.origin().y() + t * r.direction().y();
  float z = r.origin().z() + t * r.direction().z();
  if (y < y0 || y > y1 || z < z0 || z > z1)
    return false;
  rec.u = (y - y0) / (y1 - y0);
  rec.v = (z - z0) / (z1 - z0);
  rec.t = t;
  rec.set_face_normal(r, Vec3(1, 0, 0));
  rec.mat_ptr = mat_ptr;
  rec.p = r.at(t);
  return true;
}

} // namespace tracer