#include "tracer/geometry/mesh.h"

namespace tracer {
namespace geometry {

namespace {

constexpr int NUM_BUCKETS = 12;

struct BucketInfo {
  int count = 0;
  AABB bounds;
  bool initialized = false;

  void safe_expand(const AABB &b) {
    if (!initialized) {
      bounds = b;
      initialized = true;
    } else {
      bounds.expand(b);
    }
  }
};

struct BVHBuilder {
  Mesh &mesh;
  const std::vector<Vec3> &tri_centroids;

  // 构造时直接绑定 Mesh 引用，内部函数可任意访问所有网格数据
  BVHBuilder(Mesh &m, const std::vector<Vec3> &centroids)
      : mesh(m), tri_centroids(centroids) {}

  uint32_t build_recursive(uint32_t start, uint32_t end, int depth,
                           uint32_t &node_count) {
    uint32_t node_idx = node_count++;

    uint32_t count = end - start;

    // 阶段 A：计算当前节点包围盒
    AABB bbox, centroid_bbox;
    compute_bounds(start, end, bbox, centroid_bbox);
    mesh.nodes[node_idx].bbox = bbox;

    int axis = centroid_bbox.max_extent();
    float max_axis_length = centroid_bbox.max[axis] - centroid_bbox.min[axis];

    // 阶段 B：评估基础叶子节点终止条件
    if (count <= 8 || max_axis_length < 1e-6f || depth > 48) {
      return make_leaf_node(node_idx, start, count);
    }

    // 阶段 C：运行 Binned SAH 算法寻找最佳切分桶
    int best_split_bucket = -1;
    float min_cost =
        evaluate_sah(start, end, axis, max_axis_length, centroid_bbox.min[axis],
                     bbox, best_split_bucket);

    // 阶段 D：SAH 代价守卫，如果不值得切分，强制做成叶子
    if (count <= 4 && min_cost >= static_cast<float>(count)) {
      return make_leaf_node(node_idx, start, count);
    }

    // 阶段 E：根据最优桶执行网格内存数据重排
    uint32_t mid =
        partition_triangles(start, end, axis, max_axis_length,
                            centroid_bbox.min[axis], best_split_bucket);

    // 阶段 F：递归左右子树
    uint32_t left_child = build_recursive(start, mid, depth + 1, node_count);
    uint32_t right_child = build_recursive(mid, end, depth + 1, node_count);

    // 填充内部节点信息
    mesh.nodes[node_idx].left = left_child;
    mesh.nodes[node_idx].right = right_child;
    mesh.nodes[node_idx].count = 0; // 0 表示内部节点
    mesh.nodes[node_idx].axis = static_cast<uint32_t>(axis);

    return node_idx;
  }

private:
  void compute_bounds(uint32_t start, uint32_t end, AABB &bbox,
                      AABB &centroid_bbox) {
    uint32_t first_tri_idx = mesh.tri_indices[start];
    const uint32_t *f_idx = &mesh.indices[first_tri_idx * 3];

    bbox = AABB(mesh.vertices[f_idx[0]].vertex, mesh.vertices[f_idx[0]].vertex);
    centroid_bbox =
        AABB(tri_centroids[first_tri_idx], tri_centroids[first_tri_idx]);

    for (uint32_t i = start; i < end; ++i) {
      uint32_t tri_idx = mesh.tri_indices[i];
      const uint32_t *idx = &mesh.indices[tri_idx * 3];
      bbox.expand(mesh.vertices[idx[0]].vertex);
      bbox.expand(mesh.vertices[idx[1]].vertex);
      bbox.expand(mesh.vertices[idx[2]].vertex);
      centroid_bbox.expand(tri_centroids[tri_idx]);
    }
  }

  uint32_t make_leaf_node(uint32_t node_idx, uint32_t start, uint32_t count) {
    mesh.nodes[node_idx].start = start;
    mesh.nodes[node_idx].count = count;
    mesh.nodes[node_idx].axis = 0;
    return node_idx;
  }

  float evaluate_sah(uint32_t start, uint32_t end, int axis,
                     float max_axis_length, float min_centroid_axis,
                     const AABB &node_bbox, int &out_best_bucket) {
    BucketInfo buckets[NUM_BUCKETS];

    // 1. 填桶
    for (uint32_t i = start; i < end; ++i) {
      uint32_t tri_idx = mesh.tri_indices[i];
      const uint32_t *idx = &mesh.indices[tri_idx * 3];

      AABB tri_bounds(mesh.vertices[idx[0]].vertex,
                      mesh.vertices[idx[0]].vertex);
      tri_bounds.expand(mesh.vertices[idx[1]].vertex);
      tri_bounds.expand(mesh.vertices[idx[2]].vertex);

      float offset =
          (tri_centroids[tri_idx][axis] - min_centroid_axis) / max_axis_length;
      int b = static_cast<int>(offset * NUM_BUCKETS);
      if (b == NUM_BUCKETS)
        b = NUM_BUCKETS - 1;
      if (b < 0)
        b = 0; // 防御性激进安全机制

      buckets[b].count++;
      buckets[b].safe_expand(tri_bounds);
    }

    // 2. 完美的动态规划扫描（前缀和/后缀和优化，不仅准确而且极快）
    AABB left_boxes[NUM_BUCKETS];
    AABB right_boxes[NUM_BUCKETS];
    int left_counts[NUM_BUCKETS];
    int right_counts[NUM_BUCKETS];

    // 正向扫描（左侧累加）
    AABB current_left_box;
    int current_left_count = 0;
    bool left_init = false;
    for (int i = 0; i < NUM_BUCKETS; ++i) {
      if (buckets[i].initialized) {
        if (!left_init) {
          current_left_box = buckets[i].bounds;
          left_init = true;
        } else {
          current_left_box.expand(buckets[i].bounds);
        }
        current_left_count += buckets[i].count;
      }
      left_boxes[i] = current_left_box;
      left_counts[i] = current_left_count;
    }

    // 反向扫描（右侧累加）
    AABB current_right_box;
    int current_right_count = 0;
    bool right_init = false;
    for (int i = NUM_BUCKETS - 1; i >= 0; --i) {
      if (buckets[i].initialized) {
        if (!right_init) {
          current_right_box = buckets[i].bounds;
          right_init = true;
        } else {
          current_right_box.expand(buckets[i].bounds);
        }
        current_right_count += buckets[i].count;
      }
      right_boxes[i] = current_right_box;
      right_counts[i] = current_right_count;
    }

    // 3. 计算各个划分面的最小代价
    float min_cost = math::INF;
    out_best_bucket = -1;
    float inv_node_area = 1.0f / node_bbox.surface_area();

    // NUM_BUCKETS 个桶共有 NUM_BUCKETS - 1 个划分面
    for (int i = 0; i < NUM_BUCKETS - 1; ++i) {
      if (left_counts[i] > 0 && right_counts[i + 1] > 0) {
        // SAH 代价公式：Cost = 常数(通常是 traversal_cost=0.125 或 1.0) +
        // (AL*NL + AR*NR) / A_total 这里采用经典的 PBRT 权重常数
        float cost =
            0.5f + (left_counts[i] * left_boxes[i].surface_area() +
                    right_counts[i + 1] * right_boxes[i + 1].surface_area()) *
                       inv_node_area;
        if (cost < min_cost) {
          min_cost = cost;
          out_best_bucket = i;
        }
      }
    }
    return min_cost;
  }

  uint32_t partition_triangles(uint32_t start, uint32_t end, int axis,
                               float max_axis_length, float min_centroid_axis,
                               int best_split_bucket) {
    auto mid_iter = std::partition(
        mesh.tri_indices.begin() + start, mesh.tri_indices.begin() + end,
        [&](uint32_t tri_idx) {
          float offset = (tri_centroids[tri_idx][axis] - min_centroid_axis) /
                         max_axis_length;
          int b = static_cast<int>(offset * NUM_BUCKETS);
          if (b == NUM_BUCKETS)
            b = NUM_BUCKETS - 1;
          return b <= best_split_bucket;
        });

    // 使用 std::distance 计算出来的 mid 是整个数组的绝对索引位置
    uint32_t mid = static_cast<uint32_t>(
        std::distance(mesh.tri_indices.begin(), mid_iter));

    // 退化保护：如果划分失败（mid 触及了区间的两端），Fallback 到中位数对半切
    if (mid <= start || mid >= end) {
      mid = start + (end - start) / 2;
      std::nth_element(
          mesh.tri_indices.begin() + start, mesh.tri_indices.begin() + mid,
          mesh.tri_indices.begin() + end, [&](uint32_t a, uint32_t b) {
            return tri_centroids[a][axis] < tri_centroids[b][axis];
          });
    }
    return mid;
  }
};

} // namespace

void Mesh::build_bvh() {
  size_t n = indices.size() / 3;
  tri_indices.resize(n);
  std::iota(tri_indices.begin(), tri_indices.end(), 0);

  std::vector<Vec3> tri_centroids(n);
  for (size_t i = 0; i < n; ++i) {
    const uint32_t *idx = &indices[i * 3];
    tri_centroids[i] = (vertices[idx[0]].vertex + vertices[idx[1]].vertex +
                        vertices[idx[2]].vertex) /
                       3.0f;
  }

  nodes.clear();
  nodes.resize(2 * n);
  uint32_t node_count = 0;

  BVHBuilder builder(*this, tri_centroids);
  builder.build_recursive(0, static_cast<uint32_t>(n), 0, node_count);
  nodes.resize(node_count);
}

bool Mesh::ray_triangle_intersect(const Ray &r, const Vec3 &v0, const Vec3 &v1,
                                  const Vec3 &v2, float &t, float &u,
                                  float &v) {
  const float EPS = 1e-9f;
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
  // static std::atomic<long long> total_tri_tests{0};
  // static std::atomic<long long> total_rays{0};
  // total_rays++;
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
      // total_tri_tests += node.count;
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
  // if (total_rays % 100000 == 0) {
  //   float avg = (float)total_tri_tests / total_rays;
  //   printf("Avg tris per ray: %.1f\n", avg);
  //   std::cout << "[Ray Debug] Origin: " << r.origin().x()
  //             << " Dir: " << r.direction().x() << " SceneBounds: ["
  //             << bbox.min.x() << ", " << bbox.max.x() << "]" << std::endl;
  // }
  if (hit_anything) {
    rec.t = t_max;
    rec.p = r.at(t_max);

    const Vertex &v0 = vertices[indices[best_tri_idx * 3]];
    const Vertex &v1 = vertices[indices[best_tri_idx * 3 + 1]];
    const Vertex &v2 = vertices[indices[best_tri_idx * 3 + 2]];

    float w = 1.0f - best_u - best_v;
    rec.normal =
        normalize(w * v0.normal + best_u * v1.normal + best_v * v2.normal);

    Vec2 tex_coord =
        w * v0.tex_coord + best_u * v1.tex_coord + best_v * v2.tex_coord;
    rec.u = tex_coord.x();
    rec.v = tex_coord.y();

    rec.tangent =
        normalize(w * v0.tangent + best_u * v1.tangent + best_v * v2.tangent);

    rec.bitangent = cross(rec.normal, rec.tangent);

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

  float A = rec.triangle_area;
  float cos_theta = fabs(dot(v, rec.normal));

  if (cos_theta < 1e-6f)
    return 0.0f;

  float d2 = rec.t * rec.t;
  return d2 / (A * cos_theta) * (1.0f / total_area);
}

Vec3 Mesh::random(const Vec3 &o) const {
  if (tri_cdf.empty() || total_area <= 0.0f)
    return Vec3(0.0f, 0.0f, 1.0f);

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

  return u * v0 + v * v1 + w * v2;
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
    v.normal = (v.normal.squared_length() > 0.0f) ? normalize(v.normal)
                                                  : Vec3(0.0f, 0.0f, 1.0f);
  }
}

void Mesh::compute_tangents() {
  // 为每个顶点初始化切线和副切线为零
  std::vector<Vec3> tangents(vertices.size(), Vec3(0.0f, 0.0f, 0.0f));

  // 遍历所有三角形
  for (uint32_t tri = 0; tri < indices.size(); tri += 3) {
    uint32_t idx0 = indices[tri];
    uint32_t idx1 = indices[tri + 1];
    uint32_t idx2 = indices[tri + 2];

    const Vertex &v0 = vertices[idx0];
    const Vertex &v1 = vertices[idx1];
    const Vertex &v2 = vertices[idx2];

    // 边向量
    Vec3 deltaPos1 = v1.vertex - v0.vertex;
    Vec3 deltaPos2 = v2.vertex - v0.vertex;

    // 纹理坐标差
    Vec2 deltaUV1 = v1.tex_coord - v0.tex_coord;
    Vec2 deltaUV2 = v2.tex_coord - v0.tex_coord;

    float det = deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x();

    if (std::abs(det) < 1e-8f)
      continue;

    float r = 1.0f / det;
    Vec3 tangent = (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r;

    // 累加贡献到三个顶点
    tangents[idx0] += tangent;
    tangents[idx1] += tangent;
    tangents[idx2] += tangent;
  }

  // 归一化并存储到 Vertex
  for (size_t i = 0; i < vertices.size(); ++i) {
    Vec3 n = vertices[i].normal;
    Vec3 t = tangents[i];
    if (t.squared_length() < 1e-12f) {
      vertices[i].tangent = Vec3(1.0f, 0.0f, 0.0f); // 默认切线
    } else {
      // 重新正交化（Gram-Schmidt）以确保 tangent 垂直于 normal
      vertices[i].tangent = normalize(t - dot(t, n) * n);
    }
    vertices[i].bitangent = normalize(cross(n, vertices[i].tangent));
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
  bool need_smooth = true;
  for (const auto &v : vertices) {
    if (v.normal.squared_length() > 0.0f) {
      need_smooth = false;
      break;
    }
  }
  if (need_smooth) {
    compute_smooth_normals();
  }
  compute_tangents();
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