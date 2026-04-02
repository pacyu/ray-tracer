#pragma once
#include "fftw3.h"
#include "tracer/math/math.h"
#include "tracer/math/drand48.h"
#include <complex>
#include <vector>

namespace tracer {
namespace physics {

using Complex = std::complex<float>;

struct OceanParams {
  int N;            // 分辨率，如 512, 1024 (必须是 2 的幂)
  float L;          // 物理世界海面的大小，如 1000.0f 米
  float wind_speed; // 风速
  float A;          // 振幅常数
  Complex wind_dir; // 风向单位向量
};

class FFTOcean {
public:
  int N;
  float L;

  // 频域数据
  std::vector<Complex> h0_tilde;
  std::vector<Complex> h0_tilde_conj;

  // 时域置换场（用于变形三角形网格）
  std::vector<float> dx;
  std::vector<float> dy;
  std::vector<float> dz;

  std::vector<float> slope_x;
  std::vector<float> slope_y;
  std::vector<float> dx_dx;
  std::vector<float> dy_dy;
  std::vector<float> dx_dy;
  std::vector<float> dy_dx;

  FFTOcean(const OceanParams &params);
  ~FFTOcean();

  // 根据时间 t 更新海浪网格
  void solve(float t);

private:
  void generate_initial_spectrum(const OceanParams &params);

  float phillips_spectrum(float k_x, float k_y, const OceanParams &p);

  void perform_ifft(const std::vector<Complex> &input,
                    std::vector<float> &output);

  fftwf_plan ifft_plan;
  fftwf_complex *fftw_in;
  fftwf_complex *fftw_out;
};

} // namespace physics
} // namespace tracer