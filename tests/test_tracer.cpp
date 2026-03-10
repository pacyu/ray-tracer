#include "tracer/tracer.h"

using namespace tracer;

void test_tracer() {
  // 创建场景
  hittable_list world;
  auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
  auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
  auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
  auto m_light = std::make_shared<DiffuseLight>(Color(4, 4, 4));

  world.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
  world.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
  world.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));
  world.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
  world.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));

  world.add(std::make_shared<Heart>(Point3(278, 278, 278), 150.f, red));

  auto light = std::make_shared<flip_face>(
      std::make_shared<XZRect>(113, 443, 127, 432, 554, m_light));
  world.add(light);

  // 渲染场景
  const int w = 600;
  const int h = 600;
  float aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
  int samples_per_pixel = 128;
  int max_depth = 10;

  // 创建一个相机
  Camera cam(w, h, samples_per_pixel, max_depth, Color(0, 0, 0),
             Point3(278., 278., -800.), Point3(278., 278., 0.),
             Vec3(0., 1., 0.), 40., aspect_ratio);

  cam.render(world, light);
}

int main() {
  test_tracer();
  std::cout << "Test passed!" << std::endl;
  return 0;
}

//   world.add(std::make_shared<Sphere>(
//       Point3(265., 265., 325.), 150.,
//       std::make_shared<Lambertian>(Color(.2, .5, 1.))));
//   world.add(std::make_shared<Sphere>(
//       Point3(260., 150., 45.), 100.,
//       std::make_shared<Dielectric>(Color(1., 1., 1.), 1.5)));
//   auto box_angle = std::make_shared<RotateY>(
//       std::make_shared<Box>(
//           Point3(0., 0., 0.), Point3(160., 160., 160.),
//           std::make_shared<Dielectric>(Color(.9, .9, .9), 1.5)),
//       45);
//   world.add(box_angle);