#include "tracer/geometry/triangle.h"

namespace tracer {
namespace geometry {

std::shared_ptr<Material> Triangle::get_material() const {
  int mat_idx = mesh_ptr->material_indices[index];
  return mesh_ptr->materials[mat_idx];
}

bool Triangle::hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const {
  uint32_t i0 = mesh_ptr->indices[index * 3];
  uint32_t i1 = mesh_ptr->indices[index * 3 + 1];
  uint32_t i2 = mesh_ptr->indices[index * 3 + 2];

  const Vec3 &v0 = mesh_ptr->vertices[i0].vertex;
  const Vec3 &v1 = mesh_ptr->vertices[i1].vertex;
  const Vec3 &v2 = mesh_ptr->vertices[i2].vertex;

  // Möller-Trumbore
  Vec3 e1 = v1 - v0;
  Vec3 e2 = v2 - v0;
  Vec3 pvec = cross(r.direction(), e2);
  float det = dot(e1, pvec);

  if (fabs(det) < 1e-8)
    return false;

  float inv_det = 1.0f / det;
  Vec3 tvec = r.origin() - v0;
  float u = dot(tvec, pvec) * inv_det;

  if (u < 0.0f || u > 1.0f)
    return false;

  Vec3 qvec = cross(tvec, e1);
  float v = dot(r.direction(), qvec) * inv_det;

  if (v < 0.0f || u + v > 1.0f)
    return false;

  float t = dot(e2, qvec) * inv_det;

  if (t < t_min || t > t_max)
    return false;

  rec.t = t;
  rec.p = r.at(t);

  int mat_idx = mesh_ptr->material_indices[index];
  rec.mat_ptr = mesh_ptr->materials[mat_idx];

  float w = 1.0f - u - v;

  const Vec3 &n0 = mesh_ptr->vertices[i0].normal;
  const Vec3 &n1 = mesh_ptr->vertices[i1].normal;
  const Vec3 &n2 = mesh_ptr->vertices[i2].normal;
  rec.normal = unit_vector(w * n0 + u * n1 + v * n2);

  Vec2 uv0 = mesh_ptr->vertices[i0].tex_coord;
  Vec2 uv1 = mesh_ptr->vertices[i1].tex_coord;
  Vec2 uv2 = mesh_ptr->vertices[i2].tex_coord;
  rec.u = uv0.x() * w + uv1.x() * u + uv2.x() * v;
  rec.v = uv0.y() * w + uv1.y() * u + uv2.y() * v;

  // 确保法线始终与射线方向相对
  rec.set_face_normal(r, rec.normal);

  float roughness = mesh_ptr->get_roughness(index, u, v, w);
  if (roughness >= 0.f) {
    rec.roughness = roughness;
  } else {
    float slope = std::sqrt(rec.normal.x() * rec.normal.x() +
                            rec.normal.y() * rec.normal.y());

    rec.roughness = std::clamp(slope, 0.02f, 0.3f);
  }

  return true;
}

bool Triangle::bounding_box(float time0, float time1, AABB &output_box) const {
  uint32_t i0 = mesh_ptr->indices[index * 3];
  uint32_t i1 = mesh_ptr->indices[index * 3 + 1];
  uint32_t i2 = mesh_ptr->indices[index * 3 + 2];

  const Vec3 &v0 = mesh_ptr->vertices[i0].vertex;
  const Vec3 &v1 = mesh_ptr->vertices[i1].vertex;
  const Vec3 &v2 = mesh_ptr->vertices[i2].vertex;

  // 找出三个顶点的 X, Y, Z 的最小值和最大值
  Vec3 min_v(std::min({v0.x(), v1.x(), v2.x()}),
             std::min({v0.y(), v1.y(), v2.y()}),
             std::min({v0.z(), v1.z(), v2.z()}));

  Vec3 max_v(std::max({v0.x(), v1.x(), v2.x()}),
             std::max({v0.y(), v1.y(), v2.y()}),
             std::max({v0.z(), v1.z(), v2.z()}));

  // 为了防止 AABB 盒子在某个轴上厚度为 0 导致 BVH 划分失败，加一个微小的厚度
  float epsilon = 0.0001f;
  if (max_v.x() - min_v.x() < epsilon) {
    min_v.e[0] -= epsilon;
    max_v.e[0] += epsilon;
  }
  if (max_v.y() - min_v.y() < epsilon) {
    min_v.e[1] -= epsilon;
    max_v.e[1] += epsilon;
  }
  if (max_v.z() - min_v.z() < epsilon) {
    min_v.e[2] -= epsilon;
    max_v.e[2] += epsilon;
  }

  output_box = AABB(min_v, max_v);
  return true;
}

float Triangle::pdf_value(const Vec3 &o, const Vec3 &v) const {
  uint32_t i0 = mesh_ptr->indices[index * 3];
  uint32_t i1 = mesh_ptr->indices[index * 3 + 1];
  uint32_t i2 = mesh_ptr->indices[index * 3 + 2];

  hit_record rec;
  // 如果这个方向没有击中三角形，概率为 0
  if (!this->hit(Ray(o, v), 0.001f, 1e9, rec))
    return 0.0f;

  const Vec3 &v0 = mesh_ptr->vertices[i0].vertex;
  const Vec3 &v1 = mesh_ptr->vertices[i1].vertex;
  const Vec3 &v2 = mesh_ptr->vertices[i2].vertex;

  // 计算三角形面积 Area = 0.5 * |(v1 - v0) x (v2 - v0)|
  float area = 0.5f * cross(v1 - v0, v2 - v0).length();

  float distance_squared = rec.t * rec.t * v.squared_length();
  float cosine = std::abs(dot(v, rec.normal) / v.length());

  if (cosine < 1e-8)
    return 0.0f;

  // 立体角 PDF = 距离平方 / (面积 * cos(theta))
  return distance_squared / (cosine * area);
}

Vec3 Triangle::random(const Vec3 &o) const {
  uint32_t i0 = mesh_ptr->indices[index * 3];
  uint32_t i1 = mesh_ptr->indices[index * 3 + 1];
  uint32_t i2 = mesh_ptr->indices[index * 3 + 2];

  const Vec3 &v0 = mesh_ptr->vertices[i0].vertex;
  const Vec3 &v1 = mesh_ptr->vertices[i1].vertex;
  const Vec3 &v2 = mesh_ptr->vertices[i2].vertex;

  // 获取均匀采样的重心坐标
  Vec3 bary = tracer::math::random_triangle_barycentric();

  // 映射到物理 3D 空间点 P
  Vec3 random_point = bary.x() * v0 + bary.y() * v1 + bary.z() * v2;

  // 返回从观察点 o 指向该随机点的方向向量
  return random_point - o;
}

} // namespace geometry
} // namespace tracer