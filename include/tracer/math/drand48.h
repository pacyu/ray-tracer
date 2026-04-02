#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <random>
#include <thread>

namespace tracer {
namespace math {

class RandomEngine {
public:
  static std::mt19937 &get_instance() {
    thread_local std::mt19937 generator([]() {
      // 组合：高精度时间戳 + 线程ID + 静态计数器
      // 彻底避开阻塞的 std::random_device{}
      static std::atomic<uint32_t> global_counter(0);
      auto now =
          std::chrono::high_resolution_clock::now().time_since_epoch().count();
      auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
      return static_cast<uint32_t>(now ^ tid ^ global_counter.fetch_add(1));
    }());
    return generator;
  }
};

// 基础的 [0, 1) 浮点随机数
inline float random_float() {
  std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
  return distribution(RandomEngine::get_instance());
}

// 指定范围的随机数
inline float random_float(float min, float max) {
  std::uniform_real_distribution<float> distribution(min, max);
  return distribution(RandomEngine::get_instance());
}

// 正态分布
inline float normal_dist(float mean, float stddev) {
  std::normal_distribution<float> distribution(mean, stddev);
  return distribution(RandomEngine::get_instance());
}

inline int random_int(int min, int max) {
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(RandomEngine::get_instance());
}

} // namespace math
} // namespace tracer