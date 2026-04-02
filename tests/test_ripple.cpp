#include "opencv2/opencv.hpp"
#include "tracer/physics/symplectic_ripple.h"

void onMouse(int event, int x, int y, int flags, void *userdata) {
  if (event == cv::EVENT_LBUTTONDOWN || (flags & cv::EVENT_FLAG_LBUTTON)) {
    tracer::physics::SymplecticRipple *water =
        static_cast<tracer::physics::SymplecticRipple *>(userdata);
    water->disturb(x, y, 5.0f); // 鼠标点击施加力
  }
}

int main() {
  const int W = 600;
  const int H = 600;
  tracer::physics::SymplecticRipple water(W, H, 1.0f, 20.0f);

  // 1. 在中心扔下三颗石子
  water.disturb(W / 2, H / 2, 5.0f);
  water.disturb(W / 3, H / 3, 3.0f);
  water.disturb(2 * W / 3, H / 2, 4.0f);

  float dt = 0.01f; // 时间步长

  cv::namedWindow("Symplectic Ocean Simulation", cv::WINDOW_AUTOSIZE);
  cv::setMouseCallback("Symplectic Ocean Simulation", onMouse, &water);

  cv::Mat display_img(H, W, CV_8UC3);

  std::cout << "--- 辛几何流体实时 OpenCV 模拟 ---" << std::endl;
  std::cout << "操作提示: 点击鼠标左键在水面激起涟漪。按 'ESC' 键退出。"
            << std::endl;

  cv::Vec3f LightDir(1.0f, 1.0f, 1.0f);
  LightDir = cv::normalize(LightDir);
  while (true) {
    water.step(dt); // 物理步进

    // 将高度场 q 映射到 OpenCV 颜色矩阵中
    for (int y = 1; y < H - 1; ++y) {
      for (int x = 1; x < W - 1; ++x) {
        int idx = y * W + x;

        // 1. 计算 X 和 Y 方向的高度梯度 (斜率)
        float dh_dx = (water.q[idx + 1] - water.q[idx - 1]) / 2.0f;
        float dh_dy =
            (water.q[(y + 1) * W + x] - water.q[(y - 1) * W + x]) / 2.0f;

        // 2. 构造海面法向量 N (高度向 z 轴正方向，这里 OpenCV 对应通道映射)
        cv::Vec3f N(-dh_dx, -dh_dy, 1.0f);
        N = cv::normalize(N);

        // 3. 计算光照强度 (Lambertian Diffuse + 基础环境光)
        float diffuse = std::max(0.1f, N.dot(LightDir));

        // 4. 颜色映射：基础海蓝色 * 光照强度
        int b = std::clamp(static_cast<int>(220 * diffuse), 0, 255);
        int g = std::clamp(static_cast<int>(120 * diffuse), 0, 255);
        int r = std::clamp(static_cast<int>(30 * diffuse), 0, 255);

        display_img.at<cv::Vec3b>(y, x) =
            cv::Vec3b(b, g, r); // OpenCV 是 BGR 格式
      }
    }

    cv::imshow("Symplectic Ocean Simulation", display_img);

    // 每帧等待 1ms 获取键盘输入，按 ESC 退出
    char key = (char)cv::waitKey(1);
    if (key == 27)
      break;
  }

  cv::destroyAllWindows();
  return 0;
}