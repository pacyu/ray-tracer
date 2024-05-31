#pragma once

#include "hittable.h"

class RotateY : public hittable {
public:
	RotateY(std::shared_ptr<hittable> p, float angle);

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& output_box) const {
		output_box = bbox;
		return hasbox;
	}

	std::shared_ptr<hittable> ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	AABB bbox;
};

RotateY::RotateY(std::shared_ptr<hittable> p, float angle)
	: ptr(p)
{
	float radians = angle * M_PI / 180.;
	sin_theta = std::sin(radians);
	cos_theta = std::cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);

	Point3 min(inf, inf, inf);
	Point3 max(-min);

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) {
				float x = i * bbox.max.x() + (1 - i) * bbox.min.x();
				float y = j * bbox.max.y() + (1 - j) * bbox.min.y();
				float z = k * bbox.max.z() + (1 - k) * bbox.min.z();
				float n_x = cos_theta * x + sin_theta * z;
				float n_z = -sin_theta * x + cos_theta * z;

				Vec3 tester(n_x, y, n_z);

				for (int c = 0; c < 3; c++) {
					min[c] = std::min(min[c], tester[c]);
					max[c] = std::max(max[c], tester[c]);
				}
			}

	bbox = AABB(min, max);
}

bool RotateY::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
	Vec3 ori = r.origin();
	Vec3 dir = r.direction();

	ori[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	ori[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	dir[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	dir[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	Ray rotate_r(ori, dir);

	if (!ptr->hit(rotate_r, t_min, t_max, rec)) return false;

	Point3 p = rec.p;
	Point3 normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotate_r, normal);
	return true;
}