#include "tracer/geometry/ocean.h"

namespace tracer {
namespace geometry {

Ocean::Ocean(physics::FFTOcean *fft_solver, std::shared_ptr<Material> water_mat,
             float lambda, float height_scale)
    : m_simulator(fft_solver), lambda(lambda), height_scale(height_scale) {
  this->materials.push_back(water_mat);
  int Nx = fft_solver->Nx;
  int Ny = fft_solver->Ny;
  grid_num = Nx * Ny;

  vertices.resize(grid_num);
  slope_x.resize(grid_num, 0.0f);
  slope_y.resize(grid_num, 0.0f);

  int num_triangles = (Nx - 1) * (Ny - 1) * 2;
  this->indices.reserve(num_triangles * 3);
  this->material_indices.reserve(num_triangles);

  float dx = fft_solver->Lx / Nx;
  float dy = fft_solver->Ly / Ny;

  // 初始状态：创建一个平整的网格，不依赖 fft_solver 的实时数据
#pragma omp parallel for schedule(static)
  for (int idx = 0; idx < grid_num; ++idx) {
    float px = (idx % Nx) * dx - fft_solver->Lx * 0.5f;
    float py = (idx / Nx) * dy - fft_solver->Ly * 0.5f;
    vertices[idx].vertex = Vec3(px, py, 0.0f); // 初始高度为 0
    vertices[idx].normal = Vec3(0, 0, 1);      // 初始法线向上
    vertices[idx].tex_coord =
        Vec2(static_cast<float>(px) / Nx, static_cast<float>(py) / Ny);
  }

  for (int y = 0; y < Ny - 1; ++y) {
    for (int x = 0; x < Nx - 1; ++x) {
      int i0 = y * Nx + x;
      int i1 = y * Nx + (x + 1);
      int i2 = (y + 1) * Nx + x;
      int i3 = (y + 1) * Nx + (x + 1);

      // 三角形 1
      this->indices.push_back(i0);
      this->indices.push_back(i1);
      this->indices.push_back(i2);
      this->material_indices.push_back(0);

      // 三角形 2
      this->indices.push_back(i1);
      this->indices.push_back(i3);
      this->indices.push_back(i2);
      this->material_indices.push_back(0);
    }
  }
}

float Ocean::get_roughness(uint32_t tri_index, float u, float v,
                           float w) const {
  if (slope_x.empty() || slope_y.empty())
    return -1.0f;

  uint32_t i0 = indices[tri_index * 3];
  uint32_t i1 = indices[tri_index * 3 + 1];
  uint32_t i2 = indices[tri_index * 3 + 2];

  float dh_dx = w * slope_x[i0] + u * slope_x[i1] + v * slope_x[i2];
  float dh_dy = w * slope_y[i0] + u * slope_y[i1] + v * slope_y[i2];
  float slope = std::sqrt(dh_dx * dh_dx + dh_dy * dh_dy);

  return std::clamp(slope * 0.5f, 0.02f, 0.3f);
}

std::shared_ptr<Material> Ocean::get_material() const {
  if (!this->materials.empty()) {
    return this->materials[0];
  }
  return nullptr;
}

void Ocean::update_at_time(float time) {
  m_simulator->solve(time);

  int Nx = m_simulator->Nx;
  int Ny = m_simulator->Ny;
  float dx = m_simulator->Lx / Nx;
  float dy = m_simulator->Ly / Ny;

#pragma omp parallel for schedule(static)
  for (int idx = 0; idx < grid_num; ++idx) {
    int x = idx % Nx;
    int y = idx / Nx;
    float orig_x = x * dx - m_simulator->Lx * 0.5f;
    float orig_y = y * dy - m_simulator->Ly * 0.5f;

    float px = orig_x + m_simulator->dx[idx] * lambda;
    float py = orig_y + m_simulator->dy[idx] * lambda;
    float pz = m_simulator->dz[idx] * height_scale;

    vertices[idx].vertex = Vec3(px, py, pz);

    float dh_dx = m_simulator->slope_x[idx] * height_scale;
    float dh_dy = m_simulator->slope_y[idx] * height_scale;

    float dDx_dx = m_simulator->dx_dx[idx] * lambda;
    float dDx_dy = m_simulator->dx_dy[idx] * lambda;
    float dDy_dx = m_simulator->dy_dx[idx] * lambda;
    float dDy_dy = m_simulator->dy_dy[idx] * lambda;

    slope_x[idx] = m_simulator->slope_x[idx] * height_scale;
    slope_y[idx] = m_simulator->slope_y[idx] * height_scale;

    // 构造水平变形后的两个切向量 (注意 Z 轴向上)
    Vec3 tangent_x(1.0f + dDx_dx, dDy_dx, dh_dx);
    Vec3 tangent_y(dDx_dy, 1.0f + dDy_dy, dh_dy);

    // 叉积算出精确法线
    Vec3 precise_normal = unit_vector(cross(tangent_x, tangent_y));

    vertices[idx].normal = precise_normal;
    this->vertices[idx].tex_coord =
        Vec2(static_cast<float>(x) / Nx, static_cast<float>(y) / Ny);
  }

  this->refit_blas();
}

} // namespace geometry
} // namespace tracer