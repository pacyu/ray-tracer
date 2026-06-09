#include "tracer/geometry/mesh.h"

namespace tracer {
namespace geometry {

void Mesh::build_bvh() {
  size_t n = indices.size() / 3;
  tri_indices.resize(n);
  std::iota(tri_indices.begin(), tri_indices.end(), 0);

  std::vector<Vec3> tri_centroids(n);
  for (size_t i = 0; i < n; ++i) {
    uint32_t i0 = indices[i * 3];
    uint32_t i1 = indices[i * 3 + 1];
    uint32_t i2 = indices[i * 3 + 2];
    tri_centroids[i] =
        (vertices[i0].vertex + vertices[i1].vertex + vertices[i2].vertex) /
        3.0f;
  }

  nodes.clear();
  nodes.reserve(2 * n);
  build_recursive(0, static_cast<uint32_t>(n), tri_centroids);
}

uint32_t Mesh::build_recursive(uint32_t start, uint32_t end,
                               const std::vector<Vec3> &tri_centroids) {
  uint32_t node_idx = static_cast<uint32_t>(nodes.size());
  nodes.emplace_back();

  uint32_t count = end - start;
  uint32_t first_tri_idx = tri_indices[start];
  uint32_t f0 = indices[first_tri_idx * 3];

  // 计算当前节点包围盒
  AABB bbox(vertices[f0].vertex, vertices[f0].vertex);
  AABB centroid_bbox(tri_centroids[first_tri_idx],
                     tri_centroids[first_tri_idx]);

  for (uint32_t i = start; i < end; ++i) {
    uint32_t tri_idx = tri_indices[i];
    bbox.expand(vertices[indices[tri_idx * 3]].vertex);
    bbox.expand(vertices[indices[tri_idx * 3 + 1]].vertex);
    bbox.expand(vertices[indices[tri_idx * 3 + 2]].vertex);
    centroid_bbox.expand(tri_centroids[tri_idx]);
  }

  nodes[node_idx].bbox = bbox;
  int axis = centroid_bbox.max_extent();
  Vec3 extents = centroid_bbox.max - centroid_bbox.min;

  float max_axis_length = extents[axis];

  // 叶子节点设定
  if (count <= 4 || max_axis_length < 1e-6f) {
    nodes[node_idx].start = start;
    nodes[node_idx].count = count;
    nodes[node_idx].axis = 0;
    return node_idx;
  }

  uint32_t mid = start + count / 2;

  std::nth_element(tri_indices.begin() + start, tri_indices.begin() + mid,
                   tri_indices.begin() + end, [&](uint32_t a, uint32_t b) {
                     return tri_centroids[a][axis] < tri_centroids[b][axis];
                   });

  uint32_t left_child = build_recursive(start, mid, tri_centroids);
  uint32_t right_child = build_recursive(mid, end, tri_centroids);

  nodes[node_idx].left = left_child;
  nodes[node_idx].right = right_child;
  nodes[node_idx].count = 0; // 标记为内部节点
  nodes[node_idx].axis = static_cast<uint32_t>(axis);

  return node_idx;
}

bool Mesh::ray_triangle_intersect(const Ray &r, const Vec3 &v0, const Vec3 &v1,
                                  const Vec3 &v2, float &t, float &u,
                                  float &v) {
  const float EPS = 1e-8f;
  Vec3 e1 = v1 - v0;
  Vec3 e2 = v2 - v0;
  Vec3 h = cross(r.direction(), e2);
  float a = dot(e1, h);
  if (fabs(a) < EPS)
    return false;

  float f = 1.0f / a;
  Vec3 s = r.origin() - v0;
  u = f * dot(s, h);
  if (u < 0.0f || u > 1.0f)
    return false;

  Vec3 q = cross(s, e1);
  v = f * dot(r.direction(), q);
  if (v < 0.0f || u + v > 1.0f)
    return false;

  t = f * dot(e2, q);
  return t > EPS;
}

bool Mesh::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  if (nodes.empty())
    return false;

  uint32_t stack[64];
  uint32_t top = 0;
  stack[top++] = 0;

  bool hit_anything = false;
  uint32_t best_tri_idx = 0;
  float best_u = 0.0f, best_v = 0.0f;

  while (top > 0) {
    uint32_t idx = stack[--top];
    const auto &node = nodes[idx];

    if (!node.bbox.hit(r, t_min, t_max))
      continue;

    if (node.count > 0) { // 叶子节点
      for (uint32_t i = node.start; i < node.start + node.count; ++i) {
        uint32_t tri_idx = tri_indices[i];
        float t, u, v;
        if (ray_triangle_intersect(r, vertices[indices[tri_idx * 3]].vertex,
                                   vertices[indices[tri_idx * 3 + 1]].vertex,
                                   vertices[indices[tri_idx * 3 + 2]].vertex, t,
                                   u, v)) {
          if (t > t_min && t < t_max) {
            t_max = t;
            hit_anything = true;
            best_tri_idx = tri_idx;
            best_u = u;
            best_v = v;
          }
        }
      }
    } else { // 内部节点：根据光线方向决定最优压栈顺序
      uint32_t axis = node.axis;
      if (r.direction()[axis] < 0.0f) {
        stack[top++] = node.left;
        stack[top++] = node.right;
      } else {
        stack[top++] = node.right;
        stack[top++] = node.left;
      }
    }
  }

  if (hit_anything) {
    rec.t = t_max;
    rec.p = r.at(t_max);
    rec.u = best_u;
    rec.v = best_v;

    const Vertex &v0 = vertices[indices[best_tri_idx * 3]];
    const Vertex &v1 = vertices[indices[best_tri_idx * 3 + 1]];
    const Vertex &v2 = vertices[indices[best_tri_idx * 3 + 2]];

    float w = 1.0f - best_u - best_v;
    rec.normal =
        normalize(w * v0.normal + best_u * v1.normal + best_v * v2.normal);
    rec.tex_coord =
        w * v0.tex_coord + best_u * v1.tex_coord + best_v * v2.tex_coord;

    rec.triangle_idx = best_tri_idx;
    rec.triangle_area = tri_area[best_tri_idx];

    int mat_idx = material_indices[best_tri_idx];
    rec.mat_ptr = (mat_idx >= 0 && mat_idx < (int)materials.size())
                      ? materials[mat_idx]
                      : nullptr;

    rec.front_face = dot(rec.normal, r.direction()) < 0;
    if (!rec.front_face)
      rec.normal = -rec.normal;
  }

  return hit_anything;
}

bool Mesh::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = bbox;
  return true;
}

float Mesh::pdf_value(const Vec3 &o, const Vec3 &v) const {
  Ray r(o, v);
  hit_record rec;
  if (!hit(r, 0.001f, math::INF, rec))
    return 0.0f;
  if (!rec.mat_ptr)
    return 0.0f;

  float cos_theta = fabs(dot(v, rec.normal));
  if (cos_theta < 1e-6f)
    return 0.0f;

  float d2 = rec.t * rec.t;
  return d2 / (total_area * cos_theta);
}

Vec3 Mesh::random(const Vec3 &o) const {
  if (tri_cdf.empty() || total_area <= 0.0f)
    return Vec3(0, 0, 1);

  float r = math::random_float() * total_area;
  auto it = std::upper_bound(tri_cdf.begin(), tri_cdf.end(), r);
  uint32_t tri_idx = uint32_t(it - tri_cdf.begin() - 1);

  const Vec3 &v0 = vertices[indices[tri_idx * 3]].vertex;
  const Vec3 &v1 = vertices[indices[tri_idx * 3 + 1]].vertex;
  const Vec3 &v2 = vertices[indices[tri_idx * 3 + 2]].vertex;

  float r1 = math::random_float();
  float r2 = math::random_float();
  float sqrt_r1 = sqrtf(r1);
  float u = 1.0f - sqrt_r1;
  float v = r2 * sqrt_r1;
  float w = 1.0f - u - v;

  Vec3 point = u * v0 + v * v1 + w * v2;
  return normalize(point - o);
}

void Mesh::compute_smooth_normals() {
  for (size_t i = 0; i < indices.size(); i += 3) {
    int i0 = indices[i];
    int i1 = indices[i + 1];
    int i2 = indices[i + 2];

    Vec3 n = cross(vertices[i1].vertex - vertices[i0].vertex,
                   vertices[i2].vertex - vertices[i0].vertex);
    vertices[i0].normal += n;
    vertices[i1].normal += n;
    vertices[i2].normal += n;
  }

  for (Vertex &v : vertices) {
    v.normal =
        (v.normal.squared_length() > 0) ? normalize(v.normal) : Vec3(0, 0, 1);
  }
}

void Mesh::build_area_cdf() {
  uint32_t tri_count = (uint32_t)indices.size() / 3;
  tri_area.resize(tri_count);
  tri_cdf.resize(tri_count + 1);
  tri_cdf[0] = 0.0f;
  total_area = 0.0f;

  for (uint32_t i = 0; i < tri_count; ++i) {
    const Vec3 &v0 = vertices[indices[i * 3]].vertex;
    const Vec3 &v1 = vertices[indices[i * 3 + 1]].vertex;
    const Vec3 &v2 = vertices[indices[i * 3 + 2]].vertex;
    float area = 0.5f * cross(v1 - v0, v2 - v0).length();
    tri_area[i] = area;
    total_area += area;
    tri_cdf[i + 1] = total_area;
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

  build_area_cdf();
  build_bvh();
}

void Mesh::refit_bvh() {
  if (nodes.empty())
    return;
  refit_recursive(0);
}

void Mesh::refit_recursive(uint32_t node_idx) {
  BVHNode &node = nodes[node_idx];
  if (node.count > 0) {
    uint32_t f0 = indices[tri_indices[node.start] * 3];
    AABB box(vertices[f0].vertex, vertices[f0].vertex);

    for (uint32_t i = node.start; i < node.start + node.count; ++i) {
      uint32_t tri_idx = tri_indices[i];
      box.expand(vertices[indices[tri_idx * 3]].vertex);
      box.expand(vertices[indices[tri_idx * 3 + 1]].vertex);
      box.expand(vertices[indices[tri_idx * 3 + 2]].vertex);
    }
    node.bbox = box;
  } else {
    refit_recursive(node.left);
    refit_recursive(node.right);
    node.bbox =
        AABB::surrounding_box(nodes[node.left].bbox, nodes[node.right].bbox);
  }
}

void Mesh::refit_blas() {
  if (!nodes.empty()) {
    refit_bvh();
    bbox = nodes[0].bbox;
  }
}

} // namespace geometry
} // namespace tracer