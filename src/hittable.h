#pragma once
#include "ray.h"
#include "aabb.h"

class Material;

struct hit_record {
	Point3 p;
	Vec3 normal;
	std::shared_ptr<Material> mat_ptr;
	float t;
	float u;
	float v;
	bool front_face;

	void set_face_normal(const Ray& r, const Vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, AABB& output_box) const = 0;

	virtual float pdf_value(const Point3& o, const Vec3& v) const {
		return 0.0;
	}

	virtual Vec3 random(const Vec3& o) const {
		return Vec3(1, 0, 0);
	}
};

class flip_face : public hittable {
public:
	flip_face(std::shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(
		const Ray& r, float t_min, float t_max, hit_record& rec) const {

		if (!ptr->hit(r, t_min, t_max, rec))
			return false;

		rec.front_face = !rec.front_face;
		return true;
	}

	virtual bool bounding_box(float time0, float time1, AABB& output_box) const {
		return ptr->bounding_box(time0, time1, output_box);
	}

public:
	std::shared_ptr<hittable> ptr;
};
