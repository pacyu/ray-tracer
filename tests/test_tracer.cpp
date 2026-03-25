#include "tracer/tracer.h"

using namespace tracer;

static void test_tracer() {
  // 创建场景
  hittable_list world;
  auto red = std::make_shared<Lambertian>(Color(.65f, .05f, .05f));
  auto green = std::make_shared<Lambertian>(Color(.12f, .45f, .15f));
  auto gray = std::make_shared<Lambertian>(Color(.73f, .73f, .73f));
  auto earth_surface =
      std::make_shared<Lambertian>(std::make_shared<ImageTexture>(
          "C:/Users/darkchii/Downloads/earthmap.jpg"));
  auto blue = std::make_shared<Dielectric>(Color(0.6953f, 0.7422f, 0.7070f),
                                           0.1f, 1.5f);
  auto m_light = std::make_shared<DiffuseLight>(Color(4, 4, 4));

  world.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
  world.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
  world.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, gray));
  world.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, gray));
  world.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, gray));

  std::shared_ptr<hittable> heart =
      std::make_shared<Heart>(Point3(0, 0, 0), 120.f, red);
  heart = std::make_shared<RotateX>(heart, 90);
  heart = std::make_shared<Translate>(heart, Point3(278, 278, 278));
  world.add(heart);

  std::shared_ptr<hittable> sphere =
      std::make_shared<Sphere>(Point3(200, 130, 450), 80.f, earth_surface);
  world.add(sphere);

  std::shared_ptr<hittable> boll =
      std::make_shared<Sphere>(Point3(400, 130, 100), 80.f, blue);
  std::make_shared<ConstantMedium>(boll, 0.2f, Color(0.2f, 0.4f, 0.9f));
  world.add(boll);

  std::shared_ptr<hittable> box =
      std::make_shared<Box>(Point3(0, 0, 0), Point3(100, 100, 100),
                            std::make_shared<Dielectric>(
                                Color(0.6953f, 0.7422f, 0.7070f), 0.08, 1.5));
  box = std::make_shared<RotateY>(box, -18);
  box = std::make_shared<Translate>(box, Point3(130, 0, 65));
  world.add(box);

  auto light = std::make_shared<XZRect>(113, 443, 127, 432, 554, m_light);
  world.add(light);
  BVH bvh_world = BVH(world.objects, 0, world.objects.size());

  hittable_list lights;
  lights.add(light);

  // 渲染场景
  const int w = 600;
  const int h = 600;
  int samples_per_pixel = 128;
  int max_depth = 8;

  // 创建一个相机
  Camera cam(w, h, samples_per_pixel, max_depth, Color(0, 0, 0),
             Point3(278, 278, -800), Point3(278, 278, 0), Vec3(0, 1, 0), 40);

  cam.render(bvh_world, lights, false);
}

int main() {
  test_tracer();
  std::cout << "\nTest passed!" << std::endl;
  return 0;
}