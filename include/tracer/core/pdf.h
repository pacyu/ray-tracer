#pragma once
#include "tracer/core/hittable.h"
#include "tracer/math/math.h"
#include "tracer/core/onb.h"
#include "tracer/math/optics.h"
#include "tracer/math/sampling.h"

namespace tracer {

class PDF {
public:
  virtual ~PDF() {}

  virtual float value(const Vec3 &direction) const = 0;
  virtual Vec3 generate() const = 0;
};

class Cosine_pdf : public PDF {
public:
  Cosine_pdf(const Vec3 &w);

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;

public:
  onb uvw;
};

class Mixture_pdf : public PDF {
public:
  Mixture_pdf(const PDF *p0, const PDF *p1);

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;

public:
  const PDF *p[2];
};

class Hittable_pdf : public PDF {
public:
  Hittable_pdf(const hittable &p, const Point3 &origin);

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;

public:
  Point3 o;
  const hittable &ptr;
};

class Sphere_pdf : public PDF {
public:
  Sphere_pdf() {}

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;
};

class GGX_pdf : public PDF {
public:
  Vec3 n;
  Vec3 v;
  float alpha;

  GGX_pdf(const Vec3 &normal, const Vec3 &view, float alpha)
      : n(normal), v(view), alpha(alpha) {}

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;
};

} // namespace tracer