#include "tracer/tracer.h"

using namespace tracer;

static void test_tracer() {
  const int w = 600;
  const int h = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  Vec3 lookfrom = Point3(278.f, -1000.f, 278.f);
  Vec3 lookat = Point3(278.f, 0.f, 278.f);
  Vec3 vup = Vec3(0.f, 0.f, 1.f);
  const float vfov = 30.f;
  std::shared_ptr<Background> background = std::make_shared<Background>();

  // 创建场景
  hittable_list world;
  auto red = std::make_shared<material::Lambertian>(Color(.65f, .05f, .05f));
  auto green = std::make_shared<material::Lambertian>(Color(.12f, .45f, .15f));
  auto gray = std::make_shared<material::Lambertian>(Color(.73f, .73f, .73f));
  auto earth_surface = std::make_shared<material::Lambertian>(
      std::make_shared<texture::ImageTexture>(
          "../textures/Equirectangular-projection.jpg"));
  auto blue = std::make_shared<material::Dielectric>(
      Vec3(0.5294f, 0.8078f, 0.9216f), 0.2f, 1.5f);
  auto m_light = std::make_shared<material::DiffuseLight>(Color(4.f, 4.f, 4.f));

  world.add(
      std::make_shared<geometry::YZRect>(0.f, 555.f, 0.f, 555.f, 555.f, green));
  world.add(
      std::make_shared<geometry::YZRect>(0.f, 555.f, 0.f, 555.f, 0.f, red));
  world.add(
      std::make_shared<geometry::XZRect>(0.f, 555.f, 0.f, 555.f, 555.f, gray));
  world.add(
      std::make_shared<geometry::XYRect>(0.f, 555.f, 0.f, 555.f, 0.f, gray));
  world.add(
      std::make_shared<geometry::XYRect>(0.f, 555.f, 0.f, 555.f, 555.f, gray));

  std::shared_ptr<hittable> heart = std::make_shared<geometry::Heart>(
      Point3(278.f, 278.f, 278.f), 120.f, red);
  world.add(heart);

  std::shared_ptr<hittable> sphere = std::make_shared<geometry::Sphere>(
      Point3(400.f, 130.f, 250.f), 80.f, blue);
  world.add(sphere);

  std::shared_ptr<hittable> boll = std::make_shared<geometry::Sphere>(
      Point3(130.f, 350.f, 250.f), 80.f, earth_surface);
  //   boll = std::make_shared<volume::ConstantMedium>(
  //       boll, 0.2f, Vec3(0.5294f, 0.8078f, 0.9216f));
  world.add(boll);

  std::shared_ptr<hittable> box = std::make_shared<geometry::Box>(
      Point3(0.f, 0.f, 0.f), Point3(100.f, 100.f, 100.f), blue);
  box = std::make_shared<transform::RotateZ>(box, -30.f);
  box = std::make_shared<transform::Translate>(box, Point3(230.f, 0.f, 0.f));
  world.add(box);

  auto light = std::make_shared<geometry::XYRect>(113.f, 443.f, 127.f, 432.f,
                                                  554.f, m_light);
  world.add(light);
  BVH bvh_world = BVH(world);

  hittable_list lights;
  lights.add(light);

  // 创建一个相机
  Camera cam(w, h, samples_per_pixel, max_depth, "test_tracer.png", background,
             lookfrom, lookat, vup, vfov);

  // 渲染场景
  cam.render(bvh_world, lights, false);
}

int main() {
  test_tracer();
  return 0;
}