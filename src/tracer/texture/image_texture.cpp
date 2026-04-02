#include "tracer/texture/image_texture.h"

namespace tracer {
namespace texture {

ImageTexture::ImageTexture(const char *filepath) {
  image = cv::imread(filepath, cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cerr << "无法加载纹理图片: " << filepath << "，切换至纯青色纹理。"
              << std::endl;
    width = 0;
    height = 0;
  } else {
    width = image.cols;
    height = image.rows;
    std::cout << "成功加载纹理: " << filepath << " [" << width << "x" << height
              << "]" << std::endl;
  }
}

Color ImageTexture::value(float u, float v, const Point3 &p) const {
  if (image.empty())
    return Color(0, 1, 1);

  u = std::clamp(u, 0.0f, 1.0f);
  v = 1.0f - std::clamp(v, 0.0f, 1.0f);

  int i = int(u * width);
  int j = int(v * height);

  if (i >= width)
    i = width - 1;
  if (j >= height)
    j = height - 1;

  cv::Vec3b pixel = image.at<cv::Vec3b>(j, i);

  float color_scale = 1.0f / 255.0f;
  return Color(color_scale * pixel[2], color_scale * pixel[1],
               color_scale * pixel[0]);
}

} // namespace texture
} // namespace tracer