#pragma once
#include "tracer/core/texture.h"

namespace tracer {
namespace texture {

class ImageTexture : public Texture {
public:
  ImageTexture(const char *filepath);

  virtual Color value(float u, float v, const Point3 &p) const override;

private:
  int width, height;
  cv::Mat image;
};

} // namespace texture
} // namespace tracer