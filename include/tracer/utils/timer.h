#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>

namespace tracer {
namespace utils {

class RenderTimer {
public:
  explicit RenderTimer(const std::string &task_name)
      : name_(task_name), start_sys_(std::chrono::system_clock::now()),
        start_steady_(std::chrono::steady_clock::now()) {
    print_status("开始", start_sys_);
  }

  ~RenderTimer() {
    auto end_sys = std::chrono::system_clock::now();
    auto end_steady = std::chrono::steady_clock::now();

    print_status("结束", end_sys);

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_steady - start_steady_);
    std::cout << name_ << "用时: " << elapsed_ms.count() / 1000.0f << " s."
              << std::endl;
  }

private:
  void print_status(const std::string &suffix,
                    const std::chrono::system_clock::time_point &sys_time) {
    auto time_t_now = std::chrono::system_clock::to_time_t(sys_time);
    std::tm now_tm;
#ifdef _WIN32
    localtime_s(&now_tm, &time_t_now);
#else
    localtime_r(&time_t_now, &now_tm);
#endif
    std::cout << name_ + suffix
              << "! 本地时间: " << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S")
              << std::endl;
  }

  std::string name_;
  std::chrono::system_clock::time_point start_sys_;
  std::chrono::steady_clock::time_point start_steady_;
};

} // namespace utils
} // namespace tracer