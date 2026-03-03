#include <cmath>
#include <cstdio>
#include <limits>
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "heart.h"
#include "box.h"
#include "pdf.h"
#include "rotate.h"
#include "opencv2/opencv.hpp"
#include <omp.h>

#pragma warning(disable:4996)

Color ray_color(const Ray& r, const Color& background, const hittable_list& world,
  std::shared_ptr<hittable> lights, int depth)
{
  hit_record rec;

  if (depth <= 0) return background;

  if (!world.hit(r, 0.05, inf, rec)) return background;

  scatter_record srec;
  Color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, srec))
    return emitted;

  if (srec.is_specular) {
    return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth - 1);
  }

  auto light_ptr = std::make_shared<Hittable_pdf>(lights, rec.p);
  Mixture_pdf p(light_ptr, srec.pdf_ptr);

  Ray scattered = Ray(rec.p, p.generate());
  auto pdf_val = p.value(scattered.direction());

  return emitted
    + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
    * ray_color(scattered, background, world, lights, depth - 1) / pdf_val;
}

int main(int argc, char* argv[])
{
    float aspect_ratio = 1.;
    const int w = 1200,
        h = static_cast<int>(w / aspect_ratio),
        samples_per_pixel = 128,
        max_depth = 8;

    Camera cam(Point3(278., 278., -800.), Point3(278., 278., 0.), Vec3(0., 1., 0.), 40., aspect_ratio);
    Color background(.0, .0, .0);
    hittable_list world;
    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto light = std::make_shared<DiffuseLight>(Color(4, 4, 4));

    world.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    world.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    world.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));
    world.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    world.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    world.add(std::make_shared<flip_face>(std::make_shared<XZRect>(113, 443, 127, 432, 554, light)));
    world.add(std::make_shared<Heart>(Point3(278, 278, 278), 150.f, red));
    //world.add(std::make_shared<Sphere>(Point3(265., 265., 325.), 150., std::make_shared<Lambertian>(Color(.2, .5, 1.))));
    //world.add(std::make_shared<Sphere>(Point3(260., 150., 45.), 100., std::make_shared<Dielectric>(Color(1., 1., 1.), 1.5)));
    //auto box_angle = std::make_shared<RotateY>(std::make_shared<Box>(Point3(0., 0., 0.), Point3(160., 160., 160.), std::make_shared<Dielectric>(Color(.9, .9, .9), 1.5)), 45);
    //world.add(box_angle);

    auto lights = std::make_shared<hittable_list>();
    lights->add(std::make_shared<XZRect>(113, 443, 127, 432, 554, std::shared_ptr<Material>()));

    //FILE* f = fopen("image.ppm", "wb");
    //fprintf(f, "P6 %d %d %d ", w, h, 255);
    cv::Mat img = cv::Mat::zeros(cv::Size(w, h), CV_8UC3);
    auto start = std::chrono::steady_clock::now();

    #pragma omp parallel
    {
        // 每个线程拥有独立的生成器，消除竞争
        std::random_device rd;
        std::mt19937 gen(rd() ^ omp_get_thread_num());
        std::uniform_real_distribution<float> dist(0.0, 1.0);
        auto local_rand = [&]() { return dist(gen); };

        #pragma omp for schedule(dynamic)
        for (int j = h - 1; j >= 0; --j)
        {
            for (int i = 0; i < w; ++i)
            {
                Color pixel(0., 0., 0.);
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    float u = (i + local_rand()) / (w - 1),
                        v = (j + local_rand()) / (h - 1);

                    Ray r = cam.get_ray(u, v);
                    pixel += ray_color(r, background, world, lights, max_depth);
                }
                pixel /= samples_per_pixel;
                float r = std::sqrt(pixel.r());
                float g = std::sqrt(pixel.g());
                float b = std::sqrt(pixel.b());
                /*fprintf(f, "%c%c%c",
                    (uint8_t)(256 * clamp(r, 0., 0.999)),
                    (uint8_t)(256 * clamp(g, 0., 0.999)),
                    (uint8_t)(256 * clamp(b, 0., 0.999)));*/
                img.at<cv::Vec3b>(h - j - 1, i) = cv::Vec3b(
                    (256 * clamp(b, 0., 0.999)),
                    (256 * clamp(g, 0., 0.999)),
                    (256 * clamp(r, 0., 0.999))
                );
            }
            if (omp_get_thread_num() == 0 && j % 10 == 0) {
              auto now = std::chrono::steady_clock::now();
              std::chrono::duration<double> elapsed = now - start;
              float progress = (float)(h - j) / h;
              float remaining = (elapsed.count() / progress) - elapsed.count();
              printf("\r渲染进度: %.2f%% | 已用时间: %.1fs | 预计剩余: %.1fs     ", progress * 100, elapsed.count(), remaining);
            }
            //cv::imshow("Ray tracing", img);
            //if (cv::waitKey(1) == 27)
            //{
            //    cv::destroyAllWindows();
            //    break;
            //}
        }
    }
    cv::imwrite("image.png", img);

    //fclose(f);
    return 0;
}