#include "tracer/physics/fft_ocean.h"

namespace tracer {
namespace physics {

FFTOcean::FFTOcean(const OceanParams &params) : N(params.N), L(params.L) {
  h0_tilde.resize(N * N);
  h0_tilde_conj.resize(N * N);
  dx.resize(N * N, 0.0f);
  dy.resize(N * N, 0.0f);
  dz.resize(N * N, 0.0f);

  fftw_in = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * N * N);
  fftw_out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * N * N);

  // 预先测量并创建 Plan
  ifft_plan =
      fftwf_plan_dft_2d(N, N, fftw_in, fftw_out, FFTW_BACKWARD, FFTW_MEASURE);

  generate_initial_spectrum(params);
}

FFTOcean::~FFTOcean() {
  fftwf_destroy_plan(ifft_plan);
  fftwf_free(fftw_in);
  fftwf_free(fftw_out);
}

void FFTOcean::solve(float t) {
  std::vector<Complex> h_tilde_z(N * N);
  std::vector<Complex> h_tilde_x(N * N);
  std::vector<Complex> h_tilde_y(N * N);

  // 高度对 X 和 Y 的偏导数（斜率）频域
  std::vector<Complex> h_slope_x(N * N);
  std::vector<Complex> h_slope_y(N * N);

  // 水平拉扯对 X 和 Y 的偏导数频域（用于雅可比拉伸）
  std::vector<Complex> dx_dx_k(N * N);
  std::vector<Complex> dx_dy_k(N * N);
  std::vector<Complex> dy_dx_k(N * N);
  std::vector<Complex> dy_dy_k(N * N);

  int grids_num = N * N;

#pragma omp parallel for schedule(dynamic, 1)
  for (int idx = 0; idx < grids_num; ++idx) {
    int m = idx / N;
    int n = idx % N;

    int n_neg = (n == 0) ? 0 : N - n;
    int m_neg = (m == 0) ? 0 : N - m;
    int idx_neg = m_neg * N + n_neg;

    float k_x = (n <= N / 2) ? (2 * tracer::math::TRACER_PI * n / L)
                             : (2 * tracer::math::TRACER_PI * (n - N) / L);
    float k_y = (m <= N / 2) ? (2 * tracer::math::TRACER_PI * m / L)
                             : (2 * tracer::math::TRACER_PI * (m - N) / L);
    float k_len = std::sqrt(k_x * k_x + k_y * k_y);

    float omega = std::sqrt(9.81f * k_len);

    // 计算 exp(i*w*t) 和 exp(-i*w*t)
    float cos_wt = std::cos(omega * t);
    float sin_wt = std::sin(omega * t);
    Complex exp_iwt(cos_wt, sin_wt);
    Complex exp_inv_iwt(cos_wt, -sin_wt);

    h_tilde_z[idx] =
        h0_tilde[idx] * exp_iwt + h0_tilde_conj[idx_neg] * exp_inv_iwt;

    // 1. 计算高度偏导数：在频域乘以 i*k
    h_slope_x[idx] = h_tilde_z[idx] * Complex(0.0f, k_x);
    h_slope_y[idx] = h_tilde_z[idx] * Complex(0.0f, k_y);

    if (k_len > 0.0001f) {
      h_tilde_x[idx] = h_tilde_z[idx] * Complex(0.0f, -k_x / k_len);
      h_tilde_y[idx] = h_tilde_z[idx] * Complex(0.0f, -k_y / k_len);

      // 2. 计算位移偏导数：斩波位移再乘以 i*k
      dx_dx_k[idx] = h_tilde_x[idx] * Complex(0.0f, k_x);
      dx_dy_k[idx] = h_tilde_x[idx] * Complex(0.0f, k_y);
      dy_dx_k[idx] = h_tilde_y[idx] * Complex(0.0f, k_x);
      dy_dy_k[idx] = h_tilde_y[idx] * Complex(0.0f, k_y);
    } else {
      h_tilde_x[idx] = Complex(0, 0);
      h_tilde_y[idx] = Complex(0, 0);
      dx_dx_k[idx] = Complex(0, 0);
      dx_dy_k[idx] = Complex(0, 0);
      dy_dx_k[idx] = Complex(0, 0);
      dy_dy_k[idx] = Complex(0, 0);
    }
  }

  // 执行 IFFT
  perform_ifft(h_tilde_x, dx); // 水平拉扯 X
  perform_ifft(h_tilde_y, dy); // 水平拉扯 Y
  perform_ifft(h_tilde_z, dz); // 物理高度 Z

  std::vector<float> s_x(N * N), s_y(N * N);
  std::vector<float> r_dx_dx(N * N), r_dx_dy(N * N), r_dy_dx(N * N),
      r_dy_dy(N * N);

  perform_ifft(h_slope_x, s_x);
  perform_ifft(h_slope_y, s_y);

  perform_ifft(dx_dx_k, r_dx_dx);
  perform_ifft(dx_dy_k, r_dx_dy);
  perform_ifft(dy_dx_k, r_dy_dx);
  perform_ifft(dy_dy_k, r_dy_dy);

  this->slope_x = s_x;
  this->slope_y = s_y;
  this->dx_dx = r_dx_dx;
  this->dx_dy = r_dx_dy;
  this->dy_dx = r_dy_dx;
  this->dy_dy = r_dy_dy;
}

void FFTOcean::generate_initial_spectrum(const OceanParams &params) {
  int grids_num = N * N;

#pragma omp parallel for schedule(dynamic, 1)
  for (int idx = 0; idx < grids_num; ++idx) {
    int m = idx / N;
    int n = idx % N;

    float k_x = (n <= N / 2) ? (2 * tracer::math::TRACER_PI * n / L)
                             : (2 * tracer::math::TRACER_PI * (n - N) / L);
    float k_y = (m <= N / 2) ? (2 * tracer::math::TRACER_PI * m / L)
                             : (2 * tracer::math::TRACER_PI * (m - N) / L);

    float ph = phillips_spectrum(k_x, k_y, params);
    float ph_neg = phillips_spectrum(-k_x, -k_y, params);

    // 乘上高斯白噪声
    float r1 = tracer::math::normal_dist(0, 1);
    float r2 = tracer::math::normal_dist(0, 1);
    float r3 = tracer::math::normal_dist(0, 1);
    float r4 = tracer::math::normal_dist(0, 1);

    h0_tilde[idx] = Complex(r1, r2) * std::sqrt(ph / 2.0f);

    // 寻找 -k 处的共轭，保证虚部在变换后相互抵消
    h0_tilde_conj[idx] = std::conj(Complex(r3, r4) * std::sqrt(ph_neg / 2.0f));
  }
}

float FFTOcean::phillips_spectrum(float k_x, float k_y, const OceanParams &p) {
  float k_len = std::sqrt(k_x * k_x + k_y * k_y);
  if (k_len < 0.0001f)
    return 0.0f;

  float k2 = k_len * k_len;
  float k4 = k2 * k2;

  float L_val = (p.wind_speed * p.wind_speed) / 9.81f;
  float L2 = L_val * L_val;

  float wind_x = p.wind_dir.real();
  float wind_y = p.wind_dir.imag();

  float w_len = std::sqrt(wind_x * wind_x + wind_y * wind_y);

  float k_dot_w =
      (k_x / k_len) * (wind_x / w_len) + (k_y / k_len) * (wind_y / w_len);
  float k_dot_w2 = k_dot_w * k_dot_w;

  float l2 = L2 * 0.001f * 0.001f;

  if (k_dot_w < 0.0f)
    k_dot_w2 *= 0.05f;

  return p.A * (std::exp(-1.0f / (k2 * L2)) / k4) * k_dot_w2 *
         std::exp(-k2 * l2);
}

void FFTOcean::perform_ifft(const std::vector<Complex> &input,
                            std::vector<float> &output) {
#pragma omp parallel for
  for (int i = 0; i < N * N; ++i) {
    fftw_in[i][0] = input[i].real();
    fftw_in[i][1] = input[i].imag();
  }

  fftwf_execute(ifft_plan);

  float scale = 1.0f / (N * N);

#pragma omp parallel for
  for (int i = 0; i < N * N; ++i) {
    output[i] = fftw_out[i][0] * scale;
  }
}

} // namespace physics

} // namespace tracer