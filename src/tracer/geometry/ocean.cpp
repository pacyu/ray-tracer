#include "tracer/geometry/ocean.h"

namespace tracer {
namespace geometry {

Ocean::Ocean(const physics::FFTOcean &fft_solver,
             std::shared_ptr<Material> water_mat, float lambda,
             float height_scale)
    : m_simulator(fft_solver), mat_ptr(water_mat), lambda(lambda),
      height_scale(height_scale) {
  mesh = std::make_shared<TriangleMesh>(water_mat);
  int N = fft_solver.N;

  mesh->vertices.resize(N * N);
  mesh->normals.resize(N * N);
  mesh->slope_x.resize(N * N);
  mesh->slope_y.resize(N * N);

  float grid_spacing = fft_solver.L / N;

  // 初始状态：创建一个平整的网格，不依赖 fft_solver 的实时数据
#pragma omp parallel for schedule(static)
  for (int idx = 0; idx < N * N; ++idx) {
    float px = (idx % N) * grid_spacing - fft_solver.L / 2.0f;
    float py = (idx / N) * grid_spacing - fft_solver.L / 2.0f;
    mesh->vertices[idx] = Vec3(px, py, 0.0f); // 初始高度为 0
    mesh->normals[idx] = Vec3(0, 0, 1);       // 初始法线向上
    mesh->slope_x[idx] = 0.0f;
    mesh->slope_y[idx] = 0.0f;
  }

  // 构建索引并拓扑化
  std::vector<std::shared_ptr<hittable>> triangles;

  int num_faces = (N - 1) * (N - 1);
  triangles.resize(num_faces * 2);

#pragma omp parallel for schedule(static)
  for (int y = 0; y < N - 1; ++y) {
    for (int x = 0; x < N - 1; ++x) {
      int face_idx = y * (N - 1) + x;
      int i0 = y * N + x;
      int i1 = y * N + (x + 1);
      int i2 = (y + 1) * N + x;
      int i3 = (y + 1) * N + (x + 1);

      triangles[face_idx * 2 + 0] =
          std::make_shared<IndexedTriangle>(i0, i1, i2, mesh.get());

      triangles[face_idx * 2 + 1] =
          std::make_shared<IndexedTriangle>(i1, i3, i2, mesh.get());
    }
  }

  bvh_root = std::make_shared<BVH>(triangles, 0, triangles.size());
}

std::shared_ptr<Material> Ocean::get_material() const { return mat_ptr; }

bool Ocean::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  return bvh_root->hit(r, t_min, t_max, rec);
}

bool Ocean::bounding_box(float time0, float time1, AABB &output_box) const {
  return bvh_root->bounding_box(time0, time1, output_box);
}

// 将 PDF 和 Random 委托给内部的 BVH
float Ocean::pdf_value(const Vec3 &o, const Vec3 &v) const {
  return bvh_root->pdf_value(o, v);
}

Vec3 Ocean::random(const Vec3 &o) const { return bvh_root->random(o); }

void Ocean::update(float time) {
  m_simulator.solve(time);
  this->update_mesh();
  // 这里传入的时间区间 [0, 1] 通常用于运动模糊，如果没做可以传 0, 0
  bvh_root->refit(0, 0);
}

void Ocean::update_mesh() {
  int N = m_simulator.N;
  float grid_spacing = m_simulator.L / N;
  int grids_num = N * N;

#pragma omp parallel for schedule(static)
  for (int idx = 0; idx < grids_num; ++idx) {
    int x = idx % N;
    int y = idx / N;
    float orig_x = x * grid_spacing - m_simulator.L / 2.0f;
    float orig_y = y * grid_spacing - m_simulator.L / 2.0f;

    float px = orig_x + m_simulator.dx[idx] * lambda;
    float py = orig_y + m_simulator.dy[idx] * lambda;
    float pz = m_simulator.dz[idx] * height_scale;

    mesh->vertices[idx] = Vec3(px, py, pz);

    float dh_dx = m_simulator.slope_x[idx] * height_scale;
    float dh_dy = m_simulator.slope_y[idx] * height_scale;

    float dDx_dx = m_simulator.dx_dx[idx] * lambda;
    float dDx_dy = m_simulator.dx_dy[idx] * lambda;
    float dDy_dx = m_simulator.dy_dx[idx] * lambda;
    float dDy_dy = m_simulator.dy_dy[idx] * lambda;

    mesh->slope_x[idx] = m_simulator.slope_x[idx] * height_scale;
    mesh->slope_y[idx] = m_simulator.slope_y[idx] * height_scale;

    // 构造水平变形后的两个切向量 (注意 Z 轴向上)
    Vec3 tangent_x(1.0f + dDx_dx, dDy_dx, dh_dx);
    Vec3 tangent_y(dDx_dy, 1.0f + dDy_dy, dh_dy);

    // 叉积算出精确法线
    Vec3 precise_normal = unit_vector(cross(tangent_x, tangent_y));
    if (precise_normal.z() < 0)
      precise_normal = -precise_normal;

    mesh->normals[idx] = precise_normal;
  }
}

} // namespace geometry
} // namespace tracer