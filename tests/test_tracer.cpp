#include "tracer/tracer.h"

using namespace tracer;

static void test_tracer() {
  // 创建场景
  hittable_list world;
  auto red = std::make_shared<material::Lambertian>(Color(.65f, .05f, .05f));
  auto green = std::make_shared<material::Lambertian>(Color(.12f, .45f, .15f));
  auto gray = std::make_shared<material::Lambertian>(Color(.73f, .73f, .73f));
  auto earth_surface = std::make_shared<material::Lambertian>(
      std::make_shared<texture::ImageTexture>("../textures/earthmap.jpg"));
  auto blue = std::make_shared<material::Dielectric>(
      Vec3(0.5294f, 0.8078f, 0.9216f), 0.02f, 1.5f);
  auto m_light = std::make_shared<material::DiffuseLight>(Color(4, 4, 4));

  world.add(std::make_shared<geometry::YZRect>(0, 555, 0, 555, 555, green));
  world.add(std::make_shared<geometry::YZRect>(0, 555, 0, 555, 0, red));
  world.add(std::make_shared<geometry::XYRect>(0, 555, 0, 555, 555, gray));
  world.add(std::make_shared<geometry::XZRect>(0, 555, 0, 555, 0, gray));
  world.add(std::make_shared<geometry::XZRect>(0, 555, 0, 555, 555, gray));

  std::shared_ptr<hittable> heart =
      std::make_shared<geometry::Heart>(Point3(0, 0, 0), 120.f, red);
  heart = std::make_shared<transform::RotateX>(heart, 90);
  heart = std::make_shared<transform::Translate>(heart, Point3(278, 278, 278));
  world.add(heart);

  std::shared_ptr<hittable> sphere = std::make_shared<geometry::Sphere>(
      Point3(200, 130, 450), 80.f, earth_surface);
  world.add(sphere);

  std::shared_ptr<hittable> boll =
      std::make_shared<geometry::Sphere>(Point3(400, 130, 100), 80.f, blue);
  boll = std::make_shared<volume::ConstantMedium>(boll, 0.2f,
                                                  Color(0.2f, 0.4f, 0.9f));
  world.add(boll);

  std::shared_ptr<hittable> box = std::make_shared<geometry::Box>(
      Point3(0, 0, 0), Point3(100, 100, 100), blue);
  box = std::make_shared<transform::RotateY>(box, -18);
  box = std::make_shared<transform::Translate>(box, Point3(130, 0, 65));
  world.add(box);

  auto light =
      std::make_shared<geometry::XZRect>(113, 443, 127, 432, 554, m_light);
  world.add(light);
  BVH bvh_world = BVH(world);

  hittable_list lights;
  lights.add(light);

  // 渲染场景
  const int w = 600;
  const int h = 600;
  int samples_per_pixel = 128;
  int max_depth = 8;
  std::shared_ptr<Background> background = std::make_shared<Background>();

  // 创建一个相机
  Camera cam(w, h, samples_per_pixel, max_depth, "test_tracer.png", background,
             Point3(278, 278, -800), Point3(278, 278, 0), Vec3(0, 1, 0), 40);

  cam.render(bvh_world, lights, false);
}

int main() {
  test_tracer();
  std::cout << "Test passed!" << std::endl;
  return 0;
}