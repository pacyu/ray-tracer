#include "tracer/core/camera.h"

namespace tracer {

Camera::Camera()
    : image_width(600), image_height(600), samples_per_pixel(128), max_depth(8),
      output_name("image.png"),
      background(std::make_shared<PhysicalSky>(Vec3(0, 0, 0))),
      lookfrom(Vec3(1000, 0, 0)), lookat(Vec3(0, 0, 0)), vup(Vec3(0, 0, 1)),
      vfov(40.0) {
  aspect_ratio =
      static_cast<float>(image_width) / static_cast<float>(image_height);
  origin = lookfrom;
  float theta = vfov * tracer::math::TRACER_PI / 180.f;
  float h = std::tan(theta / 2.f);
  float viewport_height = 2.f * h,
        viewport_width = aspect_ratio * viewport_height;

  Vec3 w = unit_vector(lookfrom - lookat);
  Vec3 u = unit_vector(vup.cross(w));
  Vec3 v = w.cross(u);

  horizontal = viewport_width * u;
  vertical = viewport_height * v;
  lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
}

Camera::Camera(int image_width, int image_height, int samples_per_pixel,
               int max_depth, std::string name,
               std::shared_ptr<Background> background, Point3 lookfrom,
               Point3 lookat, Vec3 vup, float vfov)
    : image_width(image_width), image_height(image_height),
      samples_per_pixel(samples_per_pixel), max_depth(max_depth),
      output_name(name), background(std::move(background)), lookfrom(lookfrom),
      lookat(lookat), vup(vup), vfov(vfov) {
  aspect_ratio =
      static_cast<float>(image_width) / static_cast<float>(image_height);
  origin = lookfrom;
  float theta = vfov * tracer::math::TRACER_PI / 180.f;
  float h = std::tan(theta / 2.f);
  float viewport_height = 2.f * h,
        viewport_width = aspect_ratio * viewport_height;

  Vec3 w = unit_vector(lookfrom - lookat);
  Vec3 u = unit_vector(vup.cross(w));
  Vec3 v = w.cross(u);

  horizontal = viewport_width * u;
  vertical = viewport_height * v;
  lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
}

const Point3 Camera::get_origin() const { return origin; }

Ray Camera::get_ray(float u, float v) const {
  return Ray(origin,
             lower_left_corner + u * horizontal + v * vertical - origin);
}

void Camera::render(const hittable &world, const hittable &lights,
                    bool visual_bvh) {
  cv::Mat img = cv::Mat::zeros(cv::Size(image_width, image_height), CV_8UC3);

  auto start = std::chrono::steady_clock::now();

#pragma omp parallel for schedule(dynamic, 1)
  for (int j = image_height - 1; j >= 0; --j) {
    for (int i = 0; i < image_width; ++i) {
      Color pixel(0.f, 0.f, 0.f);
      for (int s = 0; s < samples_per_pixel; ++s) {
        float u = (i + tracer::math::random_float()) / (image_width - 1),
              v = (j + tracer::math::random_float()) / (image_height - 1);

        hit_record rec;

        Ray r = get_ray(u, v);

        r.bvh_hit_count = 0;

        if (visual_bvh) {
          world.hit(r, 0.001f, std::numeric_limits<float>::infinity(), rec);

          float heat = static_cast<float>(r.bvh_hit_count) / 50.0f;
          pixel += Color(heat, 0.0f, 0.0f); // R 红色通道代表热力
        } else {
          pixel +=
              ray_color(r, std::move(background), world, lights, max_depth);
        }
      }
      pixel /= static_cast<float>(samples_per_pixel);

      float r, g, b;

      if (visual_bvh) {
        r = pixel.r();
        g = pixel.g();
        b = pixel.b();
      } else {
        r = std::sqrt(pixel.r());
        g = std::sqrt(pixel.g());
        b = std::sqrt(pixel.b());
      }

      if (r != r)
        r = 0.0f;
      if (g != g)
        g = 0.0f;
      if (b != b)
        b = 0.0f;

      img.at<cv::Vec3b>(image_height - j - 1, i) =
          cv::Vec3b(static_cast<uchar>(256 * std::clamp(b, 0.f, 0.999f)),
                    static_cast<uchar>(256 * std::clamp(g, 0.f, 0.999f)),
                    static_cast<uchar>(256 * std::clamp(r, 0.f, 0.999f)));
    }
    if (omp_get_thread_num() == 0 && j % 10 == 0) {
      auto now = std::chrono::steady_clock::now();
      std::chrono::duration<float> elapsed = now - start;
      float progress = (float)(image_height - j) / image_height;
      float remaining = (elapsed.count() / progress) - elapsed.count();
      printf("\r渲染进度: %.2f%% | 已用时间: %.1fs | 预计剩余: %.1fs    ",
             progress * 100, elapsed.count(), remaining);
    }
  }
  printf("\n");
  cv::imwrite(visual_bvh ? "bvh_heatmap_" + output_name : output_name, img);
}

Color Camera::ray_color(const Ray &r,
                        const std::shared_ptr<Background> &background,
                        const hittable &world, const hittable &lights,
                        int depth) {
  if (depth <= 0)
    return Color(0, 0, 0);

  hit_record rec;
  bool hit_surface = world.hit(r, 0.001f, tracer::math::INF, rec);

  // bool in_water = r.origin().z() < -1.0f;

  // Medium water(Vec3(0.25f, 0.03f, 0.002f), // sigma_a
  //              Vec3(0.02f, 0.02f, 0.02f),  // sigma_s
  //              0.8f                        // g（前向散射）
  // );

  // if (in_water) {
  //   Vec3 sigma_t = water.sigma_t();
  //   float sigma_t_avg = (sigma_t.x() + sigma_t.y() + sigma_t.z()) / 3.0f;

  //   float t = -log(1 - utils::random_float()) / sigma_t_avg;

  //   if (!hit_surface || t < rec.t) {
  //     Point3 p = r.at(t);

  //     Vec3 Tr(exp(-water.sigma_a.x() * t), exp(-water.sigma_a.y() * t),
  //             exp(-water.sigma_a.z() * t));

  //     Ray scattered_ray(p, utils::sample_HG(r.direction(), water.g));
  //     return Tr *
  //            ray_color(scattered_ray, background, world, lights, depth - 1) /
  //            sigma_t_avg;
  //   } else {
  //     // 撞击表面前的吸收 (Absorption)
  //     // 即便撞到了物体，水下的光路依然要服从 Beer's Law
  //     Vec3 Tr =
  //         Vec3(exp(-water.sigma_a.x() * rec.t), exp(-water.sigma_a.y() *
  //         rec.t),
  //              exp(-water.sigma_a.z() * rec.t));
  //     return Tr * ray_color(r, background, world, lights, depth - 1);
  //   }
  // }

  if (!hit_surface) {
    return background->value(r);
  }

  scatter_record srec;
  Color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, srec))
    return emitted;

  if (srec.is_specular) {
    return srec.attenuation *
           ray_color(srec.specular_ray, background, world, lights, depth - 1);
  }

  Hittable_pdf light_ptr = Hittable_pdf(lights, rec.p);
  Mixture_pdf p(&light_ptr, srec.pdf_ptr.get());

  Ray scattered = Ray(rec.p, p.generate());
  auto pdf_val = p.value(scattered.direction());

  return emitted +
         srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
             ray_color(scattered, background, world, lights, depth - 1) /
             pdf_val;
}

} // namespace tracer