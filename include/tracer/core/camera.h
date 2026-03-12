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
  size_t image_width;
  size_t image_height;
  int samples_per_pixel;
  int max_depth;
  Vec3 background;

  Point3 lookfrom;
  Point3 lookat;
  Vec3 vup;
  float vfov;
  float aspect_ratio;

  Camera() {}
  Camera(size_t image_width, size_t image_height, int samples_per_pixel,
         int max_depth, Vec3 background, Point3 lookfrom, Point3 lookat,
         Vec3 vup, float vfov, float aspect_ratio);
  const Point3 get_origin() const;
  Ray get_ray(float u, float v) const;

  void render(const hittable_list &world, std::shared_ptr<hittable> lights);

  Color ray_color(const Ray &r, const Color &background,
                  const hittable_list &world, std::shared_ptr<hittable> lights,
                  int depth);

private:
  Point3 origin;
  Point3 lower_left_corner;
  Vec3 horizontal;
  Vec3 vertical;
};

} // namespace tracer