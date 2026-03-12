#include "tracer/core/camera.h"

namespace tracer {

Camera::Camera(size_t image_width, size_t image_height, int samples_per_pixel,
               int max_depth, Vec3 background, Point3 lookfrom, Point3 lookat,
               Vec3 vup, float vfov, float aspect_ratio)
    : image_width(image_width), image_height(image_height),
      samples_per_pixel(samples_per_pixel), max_depth(max_depth),
      background(background), lookfrom(lookfrom), lookat(lookat), vup(vup),
      vfov(vfov), aspect_ratio(aspect_ratio) {
  origin = lookfrom;
  float theta = vfov * utils::M_PI / 180.;
  float h = std::tan(theta / 2.);
  float viewport_height = 2. * h,
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

void Camera::render(const hittable_list &world,
                    std::shared_ptr<hittable> lights) {
  cv::Mat img = cv::Mat::zeros(cv::Size(image_width, image_height), CV_8UC3);

  auto start = std::chrono::steady_clock::now();
#pragma omp parallel
  {
    // 每个线程拥有独立的生成器，消除竞争
    std::random_device rd;
    std::mt19937 gen(rd() ^ omp_get_thread_num());
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    auto local_rand = [&]() { return dist(gen); };

#pragma omp for schedule(dynamic, 1)
    for (int j = image_height - 1; j >= 0; --j) {
      for (int i = 0; i < image_width; ++i) {
        Color pixel(0., 0., 0.);
        for (int s = 0; s < samples_per_pixel; ++s) {
          float u = (i + local_rand()) / (image_width - 1),
                v = (j + local_rand()) / (image_height - 1);

          Ray r = get_ray(u, v);
          pixel += ray_color(r, background, world, lights, max_depth);
        }
        pixel /= samples_per_pixel;
        float r = std::sqrt(pixel.r());
        float g = std::sqrt(pixel.g());
        float b = std::sqrt(pixel.b());

        img.at<cv::Vec3b>(image_height - j - 1, i) =
            cv::Vec3b((256 * utils::clamp(b, 0., 0.999)),
                      (256 * utils::clamp(g, 0., 0.999)),
                      (256 * utils::clamp(r, 0., 0.999)));
      }
      if (omp_get_thread_num() == 0 && j % 10 == 0) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start;
        float progress = (float)(image_height - j) / image_height;
        float remaining = (elapsed.count() / progress) - elapsed.count();
        printf("\r渲染进度: %.2f%% | 已用时间: %.1fs | 预计剩余: %.1fs    ",
               progress * 100, elapsed.count(), remaining);
      }
    }
  }

  cv::imwrite("image.png", img);
}

Color Camera::ray_color(const Ray &r, const Color &background,
                        const hittable_list &world,
                        std::shared_ptr<hittable> lights, int depth) {
  hit_record rec;

  if (depth <= 0)
    return background;

  if (!world.hit(r, 0.001, tracer::utils::inf, rec))
    return background;

  scatter_record srec;
  Color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, srec))
    return emitted;

  if (srec.is_specular) {
    return srec.attenuation *
           ray_color(srec.specular_ray, background, world, lights, depth - 1);
  }

  auto light_ptr = std::make_shared<Hittable_pdf>(lights, rec.p);
  Mixture_pdf p(light_ptr, srec.pdf_ptr);

  Ray scattered = Ray(rec.p, p.generate());
  auto pdf_val = p.value(scattered.direction());

  return emitted +
         srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
             ray_color(scattered, background, world, lights, depth - 1) /
             pdf_val;
}

} // namespace tracer