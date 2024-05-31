#pragma once

#include "vec.h"
#include "onb.h"

class PDF {
public:
  virtual ~PDF() {}

  virtual float value(const Vec3& direction) const = 0;
  virtual Vec3 generate() const = 0;
};

inline Vec3 random_cosine_direction() {
  auto r1 = random_float();
  auto r2 = random_float();
  auto z = sqrt(1 - r2);

  auto phi = 2 * M_PI * r1;
  auto x = cos(phi) * sqrt(r2);
  auto y = sin(phi) * sqrt(r2);

  return Vec3(x, y, z);
}

class Cosine_pdf : public PDF {
public:
  Cosine_pdf(const Vec3& w) { uvw.build_from_w(w); }

  virtual float value(const Vec3& direction) const override {
    auto cosine = dot(unit_vector(direction), uvw.w());
    return (cosine <= 0) ? 0 : cosine / M_PI;
  }

  virtual Vec3 generate() const override {
    return uvw.local(random_cosine_direction());
  }

public:
  onb uvw;
};

class Mixture_pdf : public PDF {
public:
  Mixture_pdf(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
    p[0] = p0;
    p[1] = p1;
  }

  virtual float value(const Vec3& direction) const override {
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
  }

  virtual Vec3 generate() const override {
    if (random_float() < 0.5)
      return p[0]->generate();
    else
      return p[1]->generate();
  }

public:
  std::shared_ptr<PDF> p[2];
};

class Hittable_pdf : public PDF {
public:
  Hittable_pdf(std::shared_ptr<hittable> p, const Point3& origin) : ptr(p), o(origin) {}

  virtual float value(const Vec3& direction) const {
    return ptr->pdf_value(o, direction);
  }

  virtual Vec3 generate() const override {
    return ptr->random(o);
  }

public:
  Point3 o;
  std::shared_ptr<hittable> ptr;
};