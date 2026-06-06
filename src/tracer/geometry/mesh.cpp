#include "tracer/geometry/mesh.h"
#include "tracer/geometry/triangle.h"

namespace tracer {
namespace geometry {

bool Mesh::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  if (!bbox.hit(r, t_min, t_max))
    return false;                              // 顶层快速过滤
  return blas_root->hit(r, t_min, t_max, rec); // 进入内部二分检索
}

bool Mesh::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = bbox;
  return true;
}

float Mesh::pdf_value(const Vec3 &o, const Vec3 &v) const {
  return blas_root->pdf_value(o, v);
}

Vec3 Mesh::random(const Vec3 &o) const { return blas_root->random(o); }

void Mesh::compute_smooth_normals() {
  for (size_t i = 0; i < indices.size(); i += 3) {
    int i0 = indices[i];
    int i1 = indices[i + 1];
    int i2 = indices[i + 2];

    const Vec3 &v0 = vertices[i0].vertex;
    const Vec3 &v1 = vertices[i1].vertex;
    const Vec3 &v2 = vertices[i2].vertex;

    Vec3 n = cross(v1 - v0, v2 - v0); // Z轴朝上逆时针叉积

    vertices[i0].normal += n;
    vertices[i1].normal += n;
    vertices[i2].normal += n;
  }

  for (Vertex &n : vertices) {
    if (n.normal.squared_length() > 0) {
      n.normal = normalize(n.normal);
    } else {
      n.normal = Vec3(0, 0, 1); // 默认 Z 轴朝上
    }
  }
}

void Mesh::finalize() {
  if (vertices.empty() || indices.empty())
    return;
  Vec3 min_p = vertices[0].vertex;
  Vec3 max_p = vertices[0].vertex;
  for (const auto &v : vertices) {
    min_p = Vec3::min(min_p, v.vertex);
    max_p = Vec3::max(max_p, v.vertex);
  }
  bbox = AABB(min_p, max_p);

  size_t triangle_count = indices.size() / 3;
  std::vector<std::shared_ptr<hittable>> local_triangles;
  local_triangles.reserve(triangle_count);

  auto self_ptr = shared_from_this();

  for (size_t i = 0; i < triangle_count; ++i) {
    local_triangles.push_back(
        std::make_shared<Triangle>(self_ptr, static_cast<uint32_t>(i)));
  }

  blas_root = std::make_shared<BVH>(local_triangles, 0, local_triangles.size());
}

void Mesh::refit_blas() {
  if (blas_root) {
    blas_root->refit(0, 0);
    blas_root->bounding_box(0, 0, bbox);
  }
}

} // namespace geometry
} // namespace tracer