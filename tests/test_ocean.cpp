#include "tracer/tracer.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

using namespace tracer;

int main() {
  // ---------------------------------------------
  // 1. 图像与渲染参数设置
  // ---------------------------------------------
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/autumn_field_puresky_4k.hdr");
  Vec3 lookfrom(0.0f, 150.0f, 50.0f);
  Vec3 lookat(0.0f, 0.0f, 2.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  auto water_mat = std::make_shared<material::OceanMaterial>(1.333f, 0.05f);
  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));

  physics::OceanParams params;
  params.N = 1024;                                // 分辨率
  params.L = 500.0f;                              // 物理大小：500米x500米
  params.wind_speed = 25.0f;                      // 风速
  params.A = 100.0f;                              // 振幅常数
  params.wind_dir = physics::Complex(1.0f, 0.0f); // 顺着 X 轴吹的风
  physics::FFTOcean fft_solver(params);

  std::cout << "[物理] 正在通过 FFTW 生成 " << params.N << "x" << params.N
            << " 离线海浪网格... " << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();

  // 实例化 Ocean 对象（内部自动建 BVH）
  auto ocean =
      std::make_shared<geometry::Ocean>(fft_solver, water_mat, 12.0f, 60.0f);

  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "[物理] 海浪与 BVH 构建完毕！用时: "
            << std::chrono::duration_cast<std::chrono::seconds>(end_time -
                                                                start_time)
                   .count()
            << " s" << std::endl;
  ocean->update(0.0f);
  hittable_list world, lights;
  auto sea_floor_mat =
      std::make_shared<material::Lambertian>(Vec3(0.01f, 0.01f, 0.02f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, 0.0f, 1000.0f),
                                                  30.0f, sun_mat);
  lights.add(light);
  world.add(light);
  world.add(ocean);
  world.add(std::make_shared<geometry::XYRect>(-1000.0f, 1000.0f, -1000.0f,
                                               1000.0f, -80.0f, sea_floor_mat));

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_ocean.png", background, lookfrom, lookat, vup, 90.0f);

  camera.render(world, lights, false);

  return 0;
}