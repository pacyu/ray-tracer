#include "tracer/core/background.h"

namespace tracer {

PhysicalSky::PhysicalSky(const Vec3 &sun_direction)
    : sun_dir(unit_vector(sun_direction)), sky_bottom(1.0f, 1.0f, 1.0f),
      sky_top(0.2f, 0.4f, 0.8f) {}

Color PhysicalSky::value(const Ray &r) const {
  Vec3 unit_dir = unit_vector(r.direction());

  // 1. 基础瑞利散射（天空由于重力/大气厚度产生的 Z 轴渐变）
  float t = 0.5f * (unit_dir.z() + 1.0f);
  Color base_sky = (1.0f - t) * sky_bottom + t * sky_top;

  // 2. 简易米氏散射（太阳周围的水雾、尘埃散射产生的圣光晕）
  float cos_theta = dot(unit_dir, sun_dir);
  Color sun_glare(0.0f, 0.0f, 0.0f);

  if (cos_theta > 0.0f) {
    // 用高次幂模拟向前的强烈散射
    float glare_factor = std::pow(cos_theta, 500.0f);
    float bloom_factor = std::pow(cos_theta, 10.0f); // 大范围微弱光晕

    sun_glare = Color(5.0f, 4.0f, 2.0f) * glare_factor +
                Color(0.5f, 0.3f, 0.1f) * bloom_factor;
  }

  return base_sky + sun_glare;
}

ImageBackground::ImageBackground(const std::string &filepath) {
  // 读取全景图（建议使用高分辨率的云空全景图）
  img = cv::imread(filepath, cv::IMREAD_ANYDEPTH | cv::IMREAD_COLOR);
  if (img.empty()) {
    std::cerr << "无法加载背景图片: " << filepath << "，切换至纯黑背景。"
              << std::endl;
    width = 0;
    height = 0;
  } else {
    width = img.cols;
    height = img.rows;
    std::cout << "成功加载HDR: " << filepath << " [" << width << "x" << height
              << "]" << std::endl;
  }
}

Color ImageBackground::value(const Ray &r) const {
  if (img.empty())
    return Color(0, 0, 0);

  Vec3 d = unit_vector(r.direction());
  float phi = std::atan2(d.y(), d.x());
  float theta = std::acos(std::clamp(d.z(), -1.0f, 1.0f));

  float u = (phi + math::TRACER_PI) / (2.0f * math::TRACER_PI);
  float v = theta / math::TRACER_PI;

  float uf = u * (width - 1);
  float vf = v * (height - 1);
  int i = static_cast<int>(uf);
  int j = static_cast<int>(vf);
  float du = uf - i;
  float dv = vf - j;

  auto get_pixel = [&](int x, int y) {
    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    cv::Vec3f bgr = img.at<cv::Vec3f>(y, x);
    // 关键：线性化处理 (Gamma 2.2 还原)
    return Color(bgr[2], bgr[1], bgr[0]);
  };

  Color c00 = get_pixel(i, j);
  Color c10 = get_pixel(i + 1, j);
  Color c01 = get_pixel(i, j + 1);
  Color c11 = get_pixel(i + 1, j + 1);

  Color lerp_color = (1 - du) * (1 - dv) * c00 + du * (1 - dv) * c10 +
                     (1 - du) * dv * c01 + du * dv * c11;

  float exposure = 1.5f;
  return lerp_color * exposure;
}

} // namespace tracer