#pragma once
#include "tracer/accelerator/bvh.h"
#include "tracer/core/hittable.h"
#include "tracer/core/material.h"
#include "tracer/math/vec2.h"
#include <numeric>

namespace tracer {
namespace geometry {

struct Vertex {
  Vec3 vertex;
  Vec3 normal;
  Vec2 tex_coord;
};

class Mesh : public hittable, public std::enable_shared_from_this<Mesh> {
public:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<uint32_t> material_indices;
  std::vector<std::shared_ptr<Material>> materials;

  std::vector<float> tri_area; // 每个三角形的面积
  std::vector<float> tri_cdf;  // 累积面积（长度为 triangle_count+1）
  float total_area = 0.0f;

  struct alignas(32) BVHNode { // 32 字节对齐，提升 cache 性能
    AABB bbox;
    uint32_t left = 0; // 子节点索引（内部节点）
    uint32_t right = 0;
    uint32_t start = 0; // 叶子节点：三角形起始索引（在 tri_indices 中）
    uint32_t count = 0; // 叶子节点：三角形数量（0 表示内部节点）
    uint32_t axis = 0;
  };
  std::vector<BVHNode> nodes;
  std::vector<uint32_t> tri_indices;

  Mesh() {}

  virtual float get_roughness(uint32_t tri_index, float u, float v,
                              float w) const {
    return -1.0f;
  }

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Vec3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Vec3 &o) const override;

  void compute_smooth_normals();
  void finalize();
  void build_bvh();
  void refit_blas();

private:
  AABB bbox;

  void build_area_cdf();
  uint32_t build_recursive(uint32_t start, uint32_t end,
                           const std::vector<Vec3> &tri_centroids);
  void refit_bvh();
  void refit_recursive(uint32_t node_idx);
  static bool ray_triangle_intersect(const Ray &r, const Vec3 &v0,
                                     const Vec3 &v1, const Vec3 &v2, float &t,
                                     float &u, float &v);
};

} // namespace geometry
} // namespace tracer