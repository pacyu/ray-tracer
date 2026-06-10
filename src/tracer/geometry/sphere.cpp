#include "tracer/geometry/sphere.h"

namespace tracer {
namespace geometry {

void get_sphere_uv(const Point3 &p, float &u, float &v) {
  auto phi = atan2(p.z(), p.x());
  auto theta = asin(p.y());
  u = 1 - (phi + tracer::math::TRACER_PI) / (2 * tracer::math::TRACER_PI);
  v = (theta + tracer::math::TRACER_PI / 2) / tracer::math::TRACER_PI;
}

bool Sphere::hit(const Ray &r, float t_min, float t_max,
                 hit_record &rec) const {
  Vec3 oc = r.origin() - center;
  float a = r.direction().squared_length();
  float b = oc.dot(r.direction());
  float c = oc.squared_length() - radius * radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0.0f) {
    float sqr = std::sqrt(discriminant);
    float t = (-b - sqr) / a;
    if (t < t_min || t > t_max) {
      t = (-b + sqr) / a;
      if (t < t_min || t > t_max)
        return false;
    }
    rec.t = t;
    rec.p = r.at(t);
    auto normal = (rec.p - center) / radius;
    rec.set_face_normal(r, normal);
    get_sphere_uv(normal, rec.u, rec.v);
    float phi = 2.0f * tracer::math::TRACER_PI * rec.u; // 方位角 [0, 2π]
    // 切线方向：沿经度增加（phi 增加）的方向，世界空间
    Vec3 tangent = Vec3(-std::sin(phi), 0.0f, std::cos(phi));
    // 在极点处退化处理
    if (tangent.squared_length() < 1e-6f) {
      tangent = Vec3(1.0f, 0.0f, 0.0f);
    }
    rec.tangent = normalize(tangent);
    // 副切线：通过叉积得到，指向纬度增加的方向
    rec.bitangent = cross(rec.normal, rec.tangent);
    rec.mat_ptr = mat_ptr;
    return true;
  }
  return false;
}

bool Sphere::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = AABB(center - Vec3(radius, radius, radius),
                    center + Vec3(radius, radius, radius));
  return true;
}

float Sphere::pdf_value(const Point3 &o, const Vec3 &v) const {
  hit_record rec;
  if (!this->hit(Ray(o, v), 0.001f, tracer::math::INF, rec))
    return 0.0f;

  float cos_theta_max =
      sqrt(1.0f - radius * radius / (center - o).squared_length());
  float solid_angle = 2.0f * tracer::math::TRACER_PI * (1.0f - cos_theta_max);

  return 1.0f / solid_angle;
}

Vec3 Sphere::random(const Point3 &o) const {
  Vec3 direction = center - o;
  auto distance_squared = direction.squared_length();
  onb uvw;
  uvw.build_from_w(direction);
  return uvw.local(tracer::math::random_to_sphere(radius, distance_squared));
}

} // namespace geometry
} // namespace tracer