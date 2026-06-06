#pragma once
#include "tracer/geometry/triangle.h"
#include "tracer/physics/fft_ocean.h"

namespace tracer {
namespace geometry {

class Ocean : public Mesh {
public:
  std::vector<float> slope_x;
  std::vector<float> slope_y;
  physics::FFTOcean *m_simulator;

  Ocean(physics::FFTOcean *, std::shared_ptr<Material>, float, float);

  virtual float get_roughness(uint32_t tri_index, float u, float v,
                              float w) const override;

  virtual std::shared_ptr<Material> get_material() const override;

  void update_at_time(float time);

private:
  float lambda = 12.0f;
  float height_scale = 60.f;
};

} // namespace geometry
} // namespace tracer