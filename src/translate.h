#pragma once
#include "hittable.h"

class Translate : public hittable {
public:
	Translate(std::shared_ptr<hittable> p, const Vec3& displacement)
		: ptr(p), offset(displacement)
	{
	}

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& output_box) const;
	
	std::shared_ptr<hittable> ptr;
	Vec3 offset;
};

bool Translate::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
	Ray moved_r(r.origin() - offset, r.direction());
	if (!ptr->hit(moved_r, t_min, t_max, rec)) return false;
	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);
	return true;
}

bool Translate::bounding_box(float t0, float t1, AABB& output_box) const {
	if (!ptr->bounding_box(t0, t1, output_box)) return false;
	output_box = AABB(output_box.min + offset, output_box.max + offset);
	return true;
}