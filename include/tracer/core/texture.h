#pragma once
#include "tracer/core/vec3.h"
#include "opencv2/opencv.hpp"

namespace tracer {

class Texture {
public:
  virtual Color value(float u, float v, const Point3 &p) const = 0;
};

class SolidColor : public Texture {
public:
  SolidColor() {}
  SolidColor(Color c) : color_value(c) {}
  SolidColor(float red, float green, float blue)
      : SolidColor(Color(red, green, blue)) {}
  virtual Color value(float u, float v, const Vec3 &p) const override;

private:
  Color color_value;
};

class ImageTexture : public Texture {
public:
  ImageTexture(const char *filename);

  virtual Color value(float u, float v, const Point3 &p) const override;

private:
  cv::Mat image;
};

} // namespace tracer