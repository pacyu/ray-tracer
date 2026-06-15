#include "tracer/tracer.h"
#include <iostream>

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
  params.wind_speed = 15.0f; // 风速
  params.A = 0.001f;         // 振幅常数
  params.wind_dir = physics::Complex(1.0f, 1.0f);
  auto fft_solver = std::make_unique<physics::FFTOcean>(nx, ny, params);

  auto water_mat = std::make_shared<material::Water>();
  //   auto water_mat =
  //       std::make_shared<material::Metal>(Vec3(0.5294f, 0.8078f, 0.9216f),
  //       0.8f);
  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, 0.0f, 1000.0f),
                                                  30.0f, sun_mat);
  auto ocean =
      std::make_shared<geometry::Ocean>(&*fft_solver, water_mat, 1.2f, 1.0f);
  ocean->update_at_time(0.0f);
  ocean->finalize();

  auto sea_floor_mat =
      std::make_shared<material::Lambertian>(Vec3(0.01f, 0.01f, 0.02f));

  hittable_list world;
  hittable_list lights;
  world.add(std::make_shared<geometry::XYRect>(
      -15000.0f, 15000.0f, -15000.0f, 15000.0f, -5000.0f, sea_floor_mat));
  world.add(ocean);
  lights.add(light);

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "image.png", background, lookfrom, lookat, vup, 60.0f);

  int fps = 60;
  float delta_t = 1.f / static_cast<float>(fps);
  int duration = fps * 10;
  float time = 0.0f;
  for (int i = 0; i < duration; i++, time += delta_t) {
    camera.output_name = "image_" + std::to_string(i) + ".png";
    std::cout << "正在渲染第 [" + std::to_string(i) + "] 帧..." << std::endl;
    ocean->update_at_time(time);
    camera.render(world, lights, false);
  }
  return 0;
}