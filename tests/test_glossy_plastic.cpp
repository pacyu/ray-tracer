#include "tracer/tracer.h"
#include <iostream>

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 50;
  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/kloppenheim_07_puresky_4k.hdr");
  Vec3 lookfrom(100.0f, 100.0f, 20.0f);
  Vec3 lookat(0.0f, 0.0f, 0.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  auto sun = std::make_shared<material::DiffuseLight>(Vec3(15.f, 15.f, 15.f));
  auto sun_sphere = std::make_shared<geometry::Sphere>(
      Vec3(0.f, -15000.f, 1000.f), 300.f, sun);

  auto glossy_mat =
      std::make_shared<material::GlossyPlastic>(Vec3(0.1f, 0.6f, 0.9f), 0.15f);

  float r = 200.0f;
  float y0 = -(r - lookat.y()) / 2.0f;
  float y1 = -y0;
  float z0 = -(r - lookat.z()) / 2.0f;
  float z1 = -z0;

  auto test_glossy =
      std::make_shared<geometry::YZRect>(y0, y1, z0, z1, 0.1f, glossy_mat);

  hittable_list world, lights;
  world.add(test_glossy);

  lights.add(sun_sphere);
  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_glossy.png", background, lookfrom, lookat, vup, 90.0f);
  camera.render(world, lights, false);
  return 0;
}