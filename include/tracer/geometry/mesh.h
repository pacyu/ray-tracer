#pragma once
#include "tracer/accelerator/bvh.h"
#include "tracer/core/hittable.h"
#include "tracer/math/vec2.h"

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

  std::shared_ptr<BVH> blas_root;

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
  void refit_blas();

private:
  AABB bbox;
};

} // namespace geometry
} // namespace tracer