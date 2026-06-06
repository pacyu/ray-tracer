#pragma once
#include "tracer/core/hittable.h"
#include "tracer/geometry/mesh.h"
#include "tracer/math/sampling.h"

namespace tracer {
namespace geometry {

class Mesh;

class Triangle : public hittable {
public:
  std::shared_ptr<tracer::geometry::Mesh> mesh_ptr;
  uint32_t index;

  Triangle(const std::shared_ptr<tracer::geometry::Mesh> &mesh, uint32_t idx)
      : mesh_ptr(mesh), index(idx) {}

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