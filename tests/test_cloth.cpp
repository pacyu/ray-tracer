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
  Vec3 lookfrom(1000.0f, 1000.0f, 50.0f);
  Vec3 lookat(0.0f, 0.0f, 2.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  auto sun = std::make_shared<material::DiffuseLight>(Vec3(15, 15, 15));
  auto sun_sphere =
      std::make_shared<geometry::Sphere>(Vec3(0, -15000, 1000), 300, sun);
  auto cloth_mat =
      std::make_shared<material::Cloth>(Vec3(0.8f, 0.1f, 0.1f), 0.1);
  auto opaque_mat = std::make_shared<material::Plastic>(
      Vec3(0.8f, 0.05f, 0.05f), 0.02f, 1.5f);
  auto glossy_mat =
      std::make_shared<material::GlossyPlastic>(Vec3(0.1f, 0.6f, 0.9f), 0.15f);
  auto glass_mat = std::make_shared<material::Dielectric>(
      Vec3(0.5294f, 0.8078f, 0.9216f), 0.02f, 1.5f);
  auto metal_mat =
      std::make_shared<material::Metal>(Color(0.06f, 0.32f, 0.73f), 0.1f);

  Vec3 w = unit_vector(lookfrom - lookat);
  Vec3 u = unit_vector(cross(vup, w));

  float r = 300.0f;       // 稍微增大一点半径
  float spacing = 800.0f; // 保持足够的间距防止重叠

  auto sphere = std::make_shared<geometry::Sphere>(lookat - 2 * u * spacing, r,
                                                   cloth_mat);
  auto plastic_sphere =
      std::make_shared<geometry::Sphere>(lookat - u * spacing, r, opaque_mat);
  auto glossy_sphere =
      std::make_shared<geometry::Sphere>(lookat, r, glossy_mat);
  auto glass_sphere =
      std::make_shared<geometry::Sphere>(lookat + u * spacing, r, glass_mat);
  auto metal_sphere = std::make_shared<geometry::Sphere>(
      lookat + 2 * u * spacing, r, metal_mat);
  hittable_list world, lights;
  world.add(sphere);
  world.add(plastic_sphere);
  world.add(glossy_sphere);
  world.add(glass_sphere);
  world.add(metal_sphere);
  lights.add(sun_sphere);
  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_cloth.png", background, lookfrom, lookat, vup, 90.0f);
  camera.render(world, lights, false);
  return 0;
}