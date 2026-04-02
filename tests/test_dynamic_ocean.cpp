#include "tracer/tracer.h"
#include <iostream>

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/autumn_field_puresky_4k.hdr");
  Vec3 lookfrom(0.0f, 30.0f, 35.0f);
  Vec3 lookat(0.0f, 0.0f, 2.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  physics::OceanParams params;
  params.N = 256;                                 // 分辨率
  params.L = 50.0f;                               // 物理大小：50米x50米
  params.wind_speed = 25.0f;                      // 风速
  params.A = 100.0f;                              // 振幅常数
  params.wind_dir = physics::Complex(1.0f, 1.0f); // 顺着 X 轴吹的风
  physics::FFTOcean fft_solver(params);

  // auto water_mat = std::make_shared<OceanMaterial>(1.333f, 0.05f);
  auto water_mat =
      std::make_shared<material::Metal>(Vec3(0.5294, 0.8078, 0.9216), 0.8);
  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, 0.0f, 1000.0f),
                                                  30.0f, sun_mat);
  auto ocean =
      std::make_shared<geometry::Ocean>(fft_solver, water_mat, 12.0f, 60.0f);
  hittable_list world;
  world.add(ocean);
  hittable_list lights;
  lights.add(light);

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "image.png", background, lookfrom, lookat, vup, 90.0f);

  int fps = 30;
  float delta_t = 1.f / static_cast<float>(fps);
  int duration = fps * 2;
  float time = 0.0f;
  for (int i = 0; i < duration; i++, time += delta_t) {
    camera.output_name = "image_" + std::to_string(i) + ".png";
    std::cout << "正在渲染第 [" + std::to_string(i) + "] 帧..." << std::endl;
    ocean->update(time);
    camera.render(world, lights, false);
  }
  return 0;
}