#pragma once
#include "tracer/core/hittable.h"
#include "tracer/core/onb.h"
#include "tracer/math/math.h"
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
  Mixture_pdf(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1);
  Mixture_pdf(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1, float blend);

  virtual float value(const Vec3 &direction) const override;

  virtual Vec3 generate() const override;

public:
  std::shared_ptr<PDF> p0, p1;
  float blend;
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

class Charlie_pdf : public PDF {
public:
  Charlie_pdf(const Vec3 &normal, const Vec3 &view, float roughness,
              const Vec3 &tangent, const Vec3 &bitangent);

  virtual float value(const Vec3 &direction) const override;
  virtual Vec3 generate() const override;

private:
  Vec3 n;      // 法线
  Vec3 v;      // 视线方向 (指向外)
  float alpha; // 粗糙度 (Roughness)
  Vec3 T, B;   // 切线空间基向量 (右手系: T, B, N)
};

} // namespace tracer