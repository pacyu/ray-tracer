#include "tracer/tracer.h"
#include <chrono>
#include <iostream>

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  Vec3 lookfrom(3.0f, 3.0f, 3.0f);
  Vec3 lookat(0.0f, 0.0f, 0.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  hittable_list world;
  hittable_list lights;

  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/autumn_field_puresky_4k.hdr");
  obj_parser::Object obj("../models/free-datsun-280z/Datsun_280Z.obj");

  std::shared_ptr<geometry::Mesh> mesh = obj.take();
  size_t vn = mesh->indices.size();
  std::cout << "模型顶点个数: " << vn << ", 面数量: " << vn / 3 << std::endl;
  std::cout << "[Build] 开始构建 BVH ..." << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();

  mesh->finalize();
  auto car = std::make_shared<transform::RotateZ>(
      std::make_shared<transform::RotateX>(
          std::make_shared<transform::Translate>(mesh, Vec3(0.f, 0.f, 0.f)),
          -90.f),
      -120.f);
  world.add(car);

  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "[Build] BVH 构建完毕！用时: "
            << std::chrono::duration_cast<std::chrono::seconds>(end_time -
                                                                start_time)
                   .count()
            << " s" << std::endl;

  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, 0.0f, 1000.0f),
                                                  30.0f, sun_mat);

  lights.add(light);

  BVH bvh(world);

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_obj_image.png", background, lookfrom, lookat, vup, 45.0f);
  camera.render(bvh, lights, false);
  return 0;
}