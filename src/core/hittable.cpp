#include "tracer/core/hittable.h"

namespace tracer {

void hit_record::set_face_normal(const Ray &r, const Vec3 &outward_normal) {
  front_face = dot(r.direction(), outward_normal) < 0;
  normal = front_face ? outward_normal : -outward_normal;
}

bool flip_face::hit(const Ray &r, float t_min, float t_max,
                    hit_record &rec) const {

  if (!ptr->hit(r, t_min, t_max, rec))
    return false;

  rec.front_face = !rec.front_face;
  return true;
}

bool flip_face::bounding_box(float time0, float time1, AABB &output_box) const {
  return ptr->bounding_box(time0, time1, output_box);
}

float flip_face::pdf_value(const Point3 &o, const Vec3 &v) const {
  return ptr->pdf_value(o, v);
}

Vec3 flip_face::random(const Vec3 &o) const { return ptr->random(o); }

} // namespace tracer