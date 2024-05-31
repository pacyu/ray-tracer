#pragma once
#include "ray.h"
#include "onb.h"
#include "hittable.h"

inline Vec3 random_to_sphere(float radius, float distance_squared) {
	auto r1 = random_float();
	auto r2 = random_float();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

	auto phi = 2 * M_PI * r1;
	auto x = cos(phi) * sqrt(1 - z * z);
	auto y = sin(phi) * sqrt(1 - z * z);

	return Vec3(x, y, z);
}

class Sphere : public hittable {
public:
	Sphere(Point3 center, float radius, std::shared_ptr<Material> m)
		: center(center), radius(radius), mat_ptr(m) {}

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& output_box) const;

	float pdf_value(const Point3& o, const Vec3& v) const {
		hit_record rec;
		if (!this->hit(Ray(o, v), 0.001, inf, rec))
			return 0;

		auto cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
		auto solid_angle = 2 * M_PI * (1 - cos_theta_max);

		return  1 / solid_angle;
	}

	Vec3 random(const Point3& o) const {
		Vec3 direction = center - o;
		auto distance_squared = direction.squared_length();
		onb uvw;
		uvw.build_from_w(direction);
		return uvw.local(random_to_sphere(radius, distance_squared));
	}

	Point3 center;
	float radius;
	std::shared_ptr<Material> mat_ptr;
};

void get_sphere_uv(const Point3& p, float& u, float& v)
{
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = (theta + M_PI / 2) / M_PI;
}

bool Sphere::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
	Vec3 oc = r.origin() - center;
	float a = r.direction().squared_length();
	float b = oc.dot(r.direction());
	float c = oc.squared_length() - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant > 0)
	{
		float sqr = std::sqrt(discriminant);
		float t = (-b - sqr) / a;
		if (t_min < t && t < t_max)
		{
			rec.t = t;
			rec.p = r.at(t);
			auto normal = (rec.p - center) / radius;
			rec.set_face_normal(r, normal);
			get_sphere_uv(normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		t = (-b + sqr) / a;
		if (t_min < t && t < t_max)
		{
			rec.t = t;
			rec.p = r.at(t);
			auto normal = (rec.p - center) / radius;
			rec.set_face_normal(r, normal);
			get_sphere_uv(normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool Sphere::bounding_box(float t0, float t1, AABB& output_box) const {
	output_box = AABB(center - Vec3(radius, radius, radius),
					  center + Vec3(radius, radius, radius));
	return true;
}