#pragma once
#include "ray.h"

class Camera {
public:
	Camera() {}
	Camera(
		Point3 lookfrom,
		Point3 lookat,
		Vec3 vup,
		float vfov,
		float aspect_ratio)
		: origin(lookfrom)
	{
		float theta = vfov * M_PI / 180.;
		float h = std::tan(theta / 2.);
		float viewport_height = 2. * h,
			viewport_width = aspect_ratio * viewport_height;

		Vec3 w = unit_vector(lookfrom - lookat);
		Vec3 u = unit_vector(vup.cross(w));
		Vec3 v = w.cross(u);

		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
	}

	Ray get_ray(float u, float v) const
	{
		return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

private:
	Point3 origin;
	Point3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
};