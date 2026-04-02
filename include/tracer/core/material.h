#pragma once
#include "tracer/core/hittable.h"
#include "tracer/core/onb.h"
#include "tracer/core/pdf.h"
#include "tracer/core/ray.h"
#include "tracer/core/texture.h"
#include "tracer/math/math.h"
#include "tracer/math/optics.h"
#include "tracer/math/sampling.h"

namespace tracer {

struct scatter_record {
  Ray specular_ray;
  bool is_specular;
  Color attenuation;
  std::unique_ptr<PDF> pdf_ptr;
};

class Material {
public:
  virtual ~Material() = default;

  virtual Color emitted(const Ray &r_in, const hit_record &rec, float u,
                        float v, const Point3 &p) const {
    return Color(0, 0, 0);
  }

  virtual bool is_emitter() const { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const = 0;

  virtual Color scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const {
    return Color(0, 0, 0);
  }
};

} // namespace tracer