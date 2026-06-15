#include "tracer/tracer.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  Vec3 lookfrom(0.0f, 1000.0f, 100.0f);
  Vec3 lookat(0.0f, 0.0f, 2.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);
  Vec3 forward = unit_vector(lookat - lookfrom);

  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/autumn_field_puresky_4k.hdr", forward, vup);

  int nx = 4096;
  int ny = 2048;
  physics::OceanParams params;
  params.Lx = 4000.0f; // 物理大小：Lx × Ly米
  params.Ly = 2000.0f;
  params.wind_speed = 25.0f;                      // 风速
  params.A = 0.001f;                              // 振幅常数
  params.wind_dir = physics::Complex(1.0f, 1.0f); // 顺着 XY 轴吹的风
  auto fft_solver = std::make_unique<physics::FFTOcean>(nx, ny, params);

  std::cout << "[Build] 正在通过 FFTW 生成 [" << nx << "x" << ny
            << "] 离线海浪网格... " << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();

  auto water_mat = std::make_shared<material::Water>(1.333f, 0.05f,
                                                     Vec3(0.05f, 0.10f, 0.12f));
  auto ocean =
      std::make_shared<geometry::Ocean>(&*fft_solver, water_mat, 1.2f, 1.5f);
  ocean->update_at_time(0.0f);
  ocean->finalize();

  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "[Build] 海浪与 BVH 构建完毕！用时: "
            << std::chrono::duration_cast<std::chrono::seconds>(end_time -
                                                                start_time)
                   .count()
            << " s" << std::endl;

  hittable_list world, lights;

  auto sea_floor_mat =
      std::make_shared<material::Lambertian>(Vec3(0.996f, 0.875f, 0.882f));
  // 确保海底足够的大，以保证折射的海面不会出现奇怪的斑
  auto sea_floor = std::make_shared<geometry::XYRect>(
      -15000.0f, 15000.0f, -15000.0f, 15000.0f, -5000.0f, sea_floor_mat);
  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, -3000.0f, 1000.0f),
                                                  300.0f, sun_mat);
  //   auto glow_mat =
  //       std::make_shared<material::SunGlow>(Vec3(1.0f, 0.9f,
  //       0.7f), 15.0f, 5.0f);
  //   auto light_glow_shell = std::make_shared<geometry::Sphere>(
  //       Vec3(0.0f, -3000.0f, 1000.0f), 450.0f, glow_mat);

  lights.add(light);
  //   world.add(light);
  //   world.add(light_glow_shell);
  world.add(ocean);
  world.add(sea_floor);

  BVH bvh(world);

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_ocean.png", background, lookfrom, lookat, vup, 60.0f);

  camera.render(bvh, lights, false);

  return 0;
}