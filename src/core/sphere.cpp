#include "tracer/core/sphere.h"

namespace tracer {

bool Sphere::hit(const Ray &r, float t_min, float t_max,
                 hit_record &rec) const {
  Vec3 oc = r.origin() - center;
  float a = r.direction().squared_length();
  float b = oc.dot(r.direction());
  float c = oc.squared_length() - radius * radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0) {
    float sqr = std::sqrt(discriminant);
    float t = (-b - sqr) / a;
    if (t_min < t && t < t_max) {
      rec.t = t;
      rec.p = r.at(t);
      auto normal = (rec.p - center) / radius;
      rec.set_face_normal(r, normal);
      get_sphere_uv(normal, rec.u, rec.v);
      rec.mat_ptr = mat_ptr;
      return true;
    }
    t = (-b + sqr) / a;
    if (t_min < t && t < t_max) {
      rec.t = t;
      rec.p = r.at(t);
      auto normal = (rec.p - center) / radius;
      rec.set_face_normal(r, normal);
      get_sphere_uv(normal, rec.u, rec.v);
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

bool Sphere::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(center - Vec3(radius, radius, radius),
                    center + Vec3(radius, radius, radius));
  return true;
}

} // namespace tracer