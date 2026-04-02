#pragma once
#include "tracer/core/hittable.h"
#include "tracer/math/sampling.h"
#include "tracer/physics/fft_ocean.h"

namespace tracer {
namespace geometry {

class TriangleMesh {
public:
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals; // 用于平滑海面的顶点法线
  std::shared_ptr<Material> mat_ptr;
  std::vector<float> slope_x;
  std::vector<float> slope_y;

  TriangleMesh(std::shared_ptr<Material> mat);

  void add_vertex(const Vec3 &v) { vertices.push_back(v); }
  void add_normal(const Vec3 &n) { normals.push_back(n); }
  void compute_smooth_normals(const std::vector<int> &indices);
};

class IndexedTriangle : public hittable {
public:
  int i0, i1, i2;               // 顶点索引
  const TriangleMesh *mesh_ptr; // 指向包含百万顶点的巨大网格

  IndexedTriangle(int a, int b, int c, const TriangleMesh *mesh)
      : i0(a), i1(b), i2(c), mesh_ptr(mesh) {}

  virtual std::shared_ptr<Material> get_material() const override;

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;

  virtual bool bounding_box(float t0, float t1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Vec3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Vec3 &o) const override;
};

} // namespace geometry
} // namespace tracer