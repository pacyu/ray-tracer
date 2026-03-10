#include "tracer/tracer.h"

using namespace tracer;

void test_heart() {
  // 创建一个心形对象
  Point3 center(0, 0, -1);
  float radius = 0.5;
  auto material = std::make_shared<Lambertian>(Color(1.0, 0.0, 0.0));
  auto heart = std::make_shared<Heart>(center, radius, material);

  // 创建一个场景并添加心形对象
  hittable_list world;
  world.add(heart);

  // 渲染场景
  int image_width = 400;
  int image_height = 400;
  float aspect_ratio =
      static_cast<float>(image_width) / static_cast<float>(image_height);
  int samples_per_pixel = 100;
  int max_depth = 50;

  // 创建一个相机
  Point3 lookfrom(0, 0, 0);
  Point3 lookat(0, 0, -1);
  Vec3 vup(0, 1, 0);
  float fov = 90;
  Camera cam(image_width, image_height, samples_per_pixel, max_depth,
             Color(1, 1, 1), lookfrom, lookat, vup, fov, aspect_ratio);
  cam.render(world, nullptr);
}

int main() {
  test_heart();
  std::cout << "Test passed!" << std::endl;
  return 0;
}