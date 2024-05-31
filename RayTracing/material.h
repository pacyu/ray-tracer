#pragma once
#include "ray.h"
#include "hittable.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

Vec3 random_in_unit_sphere() {
	Vec3 p;
	do {
		p = 2.0f * Vec3(random_float(), random_float(), random_float()) - Vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0f);
	return p;
}

struct scatter_record {
	Ray specular_ray;
	bool is_specular;
	Color attenuation;
	std::shared_ptr<PDF> pdf_ptr;
};

class Material {
public:
	virtual Color emitted(const Ray& r_in, const hit_record& rec, float u, float v, const Point3& p) const
	{
		return Color(0, 0, 0);
	}

	virtual bool scatter(
		const Ray& r, const hit_record& rec, scatter_record& srec) const = 0;

	virtual float scattering_pdf(
		const Ray& r_in, const hit_record& rec, const Ray& scattered
	) const {
		return 0;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const Color& a) : albedo(std::make_shared<SolidColor>(a)) {}
	Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}
	virtual bool scatter(const Ray& r, const hit_record& rec, scatter_record& srec) const
	{
		srec.is_specular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = std::make_shared<Cosine_pdf>(Cosine_pdf(rec.normal));
		return true;
	}

	float scattering_pdf(
		const Ray& r_in, const hit_record& rec, const Ray& scattered
	) const {
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine / M_PI;
	}

	std::shared_ptr<Texture> albedo;
};

Vec3 reflect(const Vec3&v, const Vec3&n)
{
	return v - 2 * dot(v, n) * n;
}

class Metal : public Material {
public:
	Metal(const Color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
	virtual bool scatter(const Ray& r, const hit_record& rec, scatter_record& srec) const
	{
		Vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
		srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		srec.attenuation = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		return true;
	}

	Color albedo;
	float fuzz;
};

Vec3 refract(const Vec3& uv, const Vec3& n, float etai_over_etat)
{
	float cos_theta = dot(-uv, n);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -std::sqrt(std::abs(1. - r_out_perp.squared_length())) * n;
	return r_out_perp + r_out_parallel;
}

float schlick(float cosine, float ref_idx)
{
	float r0 = (1. - ref_idx) / (1. + ref_idx);
	r0 *= r0;
	return r0 + (1. - r0) * std::pow((1. - cosine), 5.);
}

class Dielectric : public Material {
public:
	Dielectric(const Color& a, float ri) : albedo(a), ref_idx(ri) {}

	virtual bool scatter(const Ray& r, const hit_record& rec, scatter_record& srec) const
	{
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		srec.attenuation = Color(1.0, 1.0, 1.0);
		float etai_over_etat = rec.front_face ? (1. / ref_idx) : ref_idx;
		Vec3 unit_direction = unit_vector(r.direction());
		float cos_theta = std::min(dot(-unit_direction, rec.normal), 1.f);
		float sin_theta = std::sqrt(1. - cos_theta * cos_theta);
		if (etai_over_etat * sin_theta > 1.)
		{
			Vec3 reflected = reflect(unit_direction, rec.normal);
			srec.specular_ray = Ray(rec.p, reflected);
			return true;
		}
		float reflect_prob = schlick(cos_theta, etai_over_etat);
		if (reflect_prob > random_float())
		{
			Vec3 reflected = reflect(unit_direction, rec.normal);
			srec.specular_ray = Ray(rec.p, reflected);
			return true;
		}
		Vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		srec.specular_ray = Ray(rec.p, refracted);
		return true;
	}

	Color albedo;
	float ref_idx;
};

class DiffuseLight : public Material {
public:
	DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
	DiffuseLight(Color c) : emit(std::make_shared<SolidColor>(c)) {}
	
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const {
		return false;
	}

	virtual Color emitted(const Ray& r_in, const hit_record& rec, float u, float v, const Point3& p) const {
		return emit->value(u, v, p);
	}

	std::shared_ptr<Texture> emit;
};