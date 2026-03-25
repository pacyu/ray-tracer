#pragma once
#include "opencv2/opencv.hpp"
#include "tracer/core/hittable.h"
#include "tracer/core/hittable_list.h"
#include "tracer/core/material.h"
#include "tracer/core/pdf.h"
#include "tracer/core/ray.h"
#include "tracer/utils/math.h"
#include <chrono>
#include <omp.h>

namespace tracer {

class Camera {
public:
  int image_width;
  int image_height;
  int samples_per_pixel;
  int max_depth;
  Vec3 background;

  Point3 lookfrom;
  Point3 lookat;
  Vec3 vup;
  float vfov;
  float aspect_ratio;

  Camera();
  Camera(int image_width, int image_height, int samples_per_pixel,
         int max_depth, Vec3 background, Point3 lookfrom, Point3 lookat,
         Vec3 vup, float vfov);
  const Point3 get_origin() const;
  Ray get_ray(float u, float v) const;

  void render(const hittable &world, const hittable &lights, bool visual_bvh);

  Color ray_color(const Ray &r, const Color &background, const hittable &world,
                  const hittable &lights, int depth);

private:
  Point3 origin;
  Point3 lower_left_corner;
  Vec3 horizontal;
  Vec3 vertical;
};

} // namespace tracer