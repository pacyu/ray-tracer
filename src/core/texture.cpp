#include "tracer/core/texture.h"

namespace tracer {

Color SolidColor::value(float u, float v, const Vec3 &p) const {
  return color_value;
}

ImageTexture::ImageTexture(const char *filename) {
  image = cv::imread(filename, cv::IMREAD_COLOR);
  cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
}

Color ImageTexture::value(float u, float v, const Point3 &p) const {
  // If we have no texture data, then return solid cyan as a debugging aid.
  cv::Size size = image.size();
  if (size.height <= 0 || size.width <= 0)
    return Color(0, 1, 1);

  // Clamp input texture coordinates to [0,1] x [1,0]
  u = utils::clamp(u, 0, 1);
  v = 1.0f - utils::clamp(v, 0, 1); // Flip V to image coordinates

  int i = int(u * size.width);
  int j = int(v * size.height);

  if (i >= size.width)
    i = size.width - 1;
  if (j >= size.height)
    j = size.height - 1;

  cv::Vec3b pixel = image.at<cv::Vec3b>(j, i);

  float color_scale = 1.0f / 255.0f;
  return Color(color_scale * pixel[0], color_scale * pixel[1],
               color_scale * pixel[2]);
}

} // namespace tracer