#pragma once
#include "hittable.h"
#include <memory>
#include <vector>
#include "aabb.h"

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(std::shared_ptr<hittable> object)
		: objects{ object }
	{}

	void clear() { objects.clear(); }
	void add(std::shared_ptr<hittable> object)
	{
		objects.push_back(object);
	}

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;

	virtual bool bounding_box(float t0, float t1, AABB& output_box) const;

	float pdf_value(const Point3& o, const Vec3& v) const {
		auto weight = 1.0 / objects.size();
		auto sum = 0.0;

		for (const auto& object : objects)
			sum += weight * object->pdf_value(o, v);

		return sum;
	}

	Vec3 random(const Vec3& o) const {
		auto int_size = static_cast<int>(objects.size());
		return objects[random_int(0, int_size - 1)]->random(o);
	}

	std::vector<std::shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
	hit_record t_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for (const auto& object : objects)
	{
		if (object->hit(r, t_min, closest_so_far, t_rec))
		{
			hit_anything = true;
			closest_so_far = t_rec.t;
			rec = t_rec;
		}
	}
	return hit_anything;
}

bool hittable_list::bounding_box(float t0, float t1, AABB& output_box) const
{
	if (objects.empty()) return false;
	AABB temp_box;
	bool first_box = true;

	for (const auto& object : objects)
	{
		if (!object->bounding_box(t0, t1, temp_box)) return false;
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
		first_box = false;
	}
	return true;
}