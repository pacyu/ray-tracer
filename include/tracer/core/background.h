#pragma once
#include "opencv2/opencv.hpp"
#include "tracer/core/ray.h"
#include "tracer/math/math.h"

namespace tracer {

class Background {
public:
  virtual Color value(const Ray &r) const { return Color(0, 0, 0); };
};

// 物理大气背景（实时计算瑞利散射和米氏散射）
class PhysicalSky : public Background {
public:
  Vec3 sun_dir;
  Color sky_bottom;
  Color sky_top;

  PhysicalSky(const Vec3 &sun_direction);

  virtual Color value(const Ray &r) const override;
};

class ImageBackground : public Background {
public:
  cv::Mat img;
  int width;
  int height;
  Vec3 vup;
  Vec3 forward;
  Vec3 right;
  Vec3 up;

  ImageBackground(const std::string &, const Vec3 &, const Vec3 &);

  virtual Color value(const Ray &r) const override;
};

} // namespace tracer