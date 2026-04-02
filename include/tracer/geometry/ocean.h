#pragma once
#include "tracer/accelerator/bvh.h"
#include "tracer/geometry/triangle.h"
#include "tracer/physics/fft_ocean.h"

namespace tracer {
namespace geometry {

class Ocean : public hittable {
public:
  Ocean(const physics::FFTOcean &, std::shared_ptr<Material>, float, float);

  std::shared_ptr<Material> get_material() const override;

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &rec) const override;

  virtual bool bounding_box(float time0, float time1,
                            AABB &output_box) const override;

  virtual float pdf_value(const Vec3 &o, const Vec3 &v) const override;

  virtual Vec3 random(const Vec3 &o) const override;

  void update(float time);

  physics::FFTOcean m_simulator;
  std::shared_ptr<TriangleMesh> mesh;
  std::shared_ptr<Material> mat_ptr;
  std::shared_ptr<hittable> bvh_root;

private:
  float lambda = 12.0f;
  float height_scale = 60.f;

  void update_mesh();
};

} // namespace geometry
} // namespace tracer