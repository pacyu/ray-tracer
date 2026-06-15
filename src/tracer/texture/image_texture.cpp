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
    width = image.cols - 1;
    height = image.rows - 1;
    std::cout << "成功加载纹理: " << filepath << " [" << width + 1 << "x" << height + 1
              << "]" << std::endl;
  }
}

Color ImageTexture::value(float u, float v, const Point3 &p) const {
  if (image.empty())
    return Color(0, 1, 1);

  u = std::clamp(u, 0.0f, 1.0f);
  v = 1.0f - std::clamp(v, 0.0f, 1.0f);

  float uf = u * width;
  float vf = v * height;

  int i0 = static_cast<int>(uf);
  int j0 = static_cast<int>(vf);
  int i1 = std::min(i0 + 1, width);
  int j1 = std::min(j0 + 1, height);

  float du = uf - i0;
  float dv = vf - j0;

  // 获取双线性插值后的颜色
  auto get_pixel = [&](int x, int y) {
    cv::Vec3b b = image.at<cv::Vec3b>(y, x);
    return Color(b[2] / 255.0f, b[1] / 255.0f, b[0] / 255.0f);
  };

  Color c00 = get_pixel(i0, j0);
  Color c10 = get_pixel(i1, j0);
  Color c01 = get_pixel(i0, j1);
  Color c11 = get_pixel(i1, j1);

  Color color = (1 - du) * (1 - dv) * c00 + du * (1 - dv) * c10 +
                (1 - du) * dv * c01 + du * dv * c11;
  return color;
}

} // namespace texture
} // namespace tracer