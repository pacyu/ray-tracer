#pragma once
#include "aarect.h"
#include "hittable_list.h"

class Box : public hittable {
public:
	Box() {}
	Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> mat_ptr)
		: box_min(p0), box_max(p1)
	{
		sides.add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat_ptr));
		sides.add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat_ptr));

		sides.add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat_ptr));
		sides.add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat_ptr));

		sides.add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat_ptr));
		sides.add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat_ptr));
	}

	virtual bool hit(const Ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& output_box) const {
		output_box = AABB(box_min, box_max);
		return true;
	}

	Point3 box_min;
	Point3 box_max;
	hittable_list sides;
};

bool Box::hit(const Ray& r, float t0, float t1, hit_record& rec) const
{
	return sides.hit(r, t0, t1, rec);
}