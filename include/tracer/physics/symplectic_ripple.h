#pragma once
#include <vector>

namespace tracer {
namespace physics {

class SymplecticRipple {
public:
  int width, height;
  float dx; // 网格间距
  float c;  // 波速

  std::vector<float> q; // 高度 Height field
  std::vector<float> p; // 速度 Velocity field

  SymplecticRipple(int w, int h, float spacing, float wave_speed)
      : width(w), height(h), dx(spacing), c(wave_speed) {
    q.resize(w * h, 0.0f);
    p.resize(w * h, 0.0f);
  }

  // 辛欧拉积分步
  void step(float dt) {
    std::vector<float> next_p = p;

    // 1. 更新动量 (速度) - 利用拉普拉斯算子 (有限差分法)
    for (int y = 1; y < height - 1; ++y) {
      for (int x = 1; x < width - 1; ++x) {
        int idx = y * width + x;

        // 采集上下左右四个邻居的高度
        float q_center = q[idx];
        float q_left = q[idx - 1];
        float q_right = q[idx + 1];
        float q_up = q[(y - 1) * width + x];
        float q_down = q[(y + 1) * width + x];

        // 五点拉普拉斯算子算曲率
        float laplacian =
            (q_left + q_right + q_up + q_down - 4.0f * q_center) / (dx * dx);

        // 物理更新速度: dp/dt = c^2 * laplacian
        next_p[idx] += dt * (c * c) * laplacian;
      }
    }
    p = next_p; // 同步动量

    // 2. 更新位置 (高度) - 极其简单！使用新速度
    for (int i = 0; i < width * height; ++i) {
      q[i] += dt * p[i]; // q_{t+1} = q_t + dt * p_{t+1}
      p[i] *= 0.998f;
    }
  }

  // 在某个网格点投下一颗石子（扰动）
  void disturb(int x, int y, float amount) {
    if (x > 1 && x < width - 2 && y > 1 && y < height - 2) {
      // 扩散一下，让水滴看起来像一个高斯斑，而不是尖刺
      q[y * width + x] += amount;
      q[y * width + x + 1] += amount * 0.5f;
      q[y * width + x - 1] += amount * 0.5f;
      q[(y + 1) * width + x] += amount * 0.5f;
      q[(y - 1) * width + x] += amount * 0.5f;
    }
  }
};

} // namespace physics
} // namespace tracer