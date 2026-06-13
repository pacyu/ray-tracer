#include "tracer/tracer.h"
#include <iostream>

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 50;
  Vec3 lookfrom(100.0f, 100.0f, 20.0f);
  Vec3 lookat(0.0f, 0.0f, 0.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);
  Vec3 forward = unit_vector(lookat - lookfrom);

  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/kloppenheim_07_puresky_4k.hdr", forward, vup);

  auto sun = std::make_shared<material::DiffuseLight>(Vec3(15.f, 15.f, 15.f));
  auto sun_sphere = std::make_shared<geometry::Sphere>(
      Vec3(0.f, -15000.f, 1000.f), 300.f, sun);

  auto glass_mat = std::make_shared<material::Dielectric>(
      Vec3(0.5294f, 0.8078f, 0.9216f), 0.02f, 1.5f);

  float r = 200.0f;
  float y0 = -(r - lookat.y()) / 2.0f;
  float y1 = -y0;
  float z0 = -(r - lookat.z()) / 2.0f;
  float z1 = -z0;

  auto test_glass =
      std::make_shared<geometry::YZRect>(y0, y1, z0, z1, 0.1f, glass_mat);

  hittable_list world, lights;
  world.add(test_glass);

  lights.add(sun_sphere);
  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_glass.png", background, lookfrom, lookat, vup, 90.0f);
  camera.render(world, lights, false);
  return 0;
}