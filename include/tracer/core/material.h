#pragma once
#include "tracer/core/hittable.h"
#include "tracer/core/onb.h"
#include "tracer/core/pdf.h"
#include "tracer/core/ray.h"
#include "tracer/core/texture.h"
#include "tracer/utils/drand48.h"
#include "tracer/utils/math.h"

namespace tracer {

Vec3 random_in_unit_sphere();
Vec3 reflect(const Vec3 &v, const Vec3 &n);
Vec3 refract(const Vec3 &uv, const Vec3 &n, float etai_over_etat);
float schlick(float cosine, float ref_idx);

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

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const {
    return 0;
  }
};

class Lambertian : public Material {
public:
  Lambertian(const Color &a) : albedo(std::make_shared<SolidColor>(a)) {}
  Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const override;

  std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
  Metal(const Color &a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  Color albedo;
  float fuzz;
};

class Dielectric : public Material {
public:
  Dielectric(float refraction_index)
      : albedo(), density(), ref_idx(refraction_index) {}
  Dielectric(Color color, float refraction_index)
      : albedo(color), density(), ref_idx(refraction_index) {}
  Dielectric(Color color, float density, float refraction_index)
      : albedo(color), density(density), ref_idx(refraction_index) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r, const hit_record &rec,
                       scatter_record &srec) const override;

  Color albedo;
  float ref_idx;
  float density;
};

class DiffuseLight : public Material {
public:
  DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
  DiffuseLight(Color c) : emit(std::make_shared<SolidColor>(c)) {}

  virtual bool is_emitter() const override { return true; }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual Color emitted(const Ray &r_in, const hit_record &rec, float u,
                        float v, const Point3 &p) const override;

  std::shared_ptr<Texture> emit;
};

class Isotropic : public Material {
public:
  Isotropic(const Color &albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
  Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

  virtual bool is_emitter() const override { return false; }

  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       scatter_record &srec) const override;

  virtual float scattering_pdf(const Ray &r_in, const hit_record &rec,
                               const Ray &scattered) const override;

private:
  std::shared_ptr<Texture> tex;
};

} // namespace tracer