#include "tracer/material/ocean_material.h"

namespace tracer {
namespace material {

bool OceanMaterial::scatter(const Ray &r_in, const hit_record &rec,
                            scatter_record &srec) const {
  Vec3 n = rec.normal;
  Vec3 incident = unit_vector(r_in.direction()); // 入射方向
  Vec3 v = -incident;                            // 视线方向 (指向外部)

  float etai = 1.0f, etat = ior;
  bool is_front_face = dot(incident, n) < 0;

  // 默认情况下，如果光线在空气中飞行击中水面，能量不衰减
  Vec3 attenuation(1.0f, 1.0f, 1.0f);

  // 【新增：比尔定律体积吸收】
  if (!is_front_face) {
    // 光线是从水下往外打的，说明它刚刚在水下穿行了 rec.t 的距离
    n = -n;
    std::swap(etai, etat);

    // 定义水体的吸收系数 (Absorption Coefficient)
    // R, G, B 分别对应红、绿、蓝光的吸收率。红光最高，蓝光最低。
    // 注意：这个系数的绝对值高度依赖你的世界坐标系比例 (Scene Scale)
    Vec3 absorption_coefficient(5.0f, 2.0f, 0.5f);

    attenuation = Vec3(std::exp(-absorption_coefficient.x() * rec.t),
                       std::exp(-absorption_coefficient.y() * rec.t),
                       std::exp(-absorption_coefficient.z() * rec.t));
  }

  float alpha = std::max(0.001f, roughness * roughness);

  // 采样微表面法线 h
  Vec3 h_local = tracer::math::sample_GGX(alpha);
  onb uvw;
  uvw.build_from_w(n);
  Vec3 h = unit_vector(uvw.local(h_local));

  if (dot(h, v) < 0) {
    h = -h;
  }

  // 菲涅尔与镜面标记
  float VoH = std::max(dot(v, h), 0.0001f);
  float fresnel = tracer::optics::fresnel_schlick(VoH, etai, etat);

  srec.is_specular = true;
  // 将计算好的带有体积吸收特征的 attenuation 赋给光线
  srec.attenuation = attenuation;

  // 3. 轮盘赌：反射或折射
  if (tracer::math::random_float() < fresnel) {
    Vec3 reflected = tracer::optics::reflect(incident, h);
    srec.specular_ray = Ray(rec.p + n * 0.001f, reflected);
  } else {
    Vec3 refracted;
    if (tracer::optics::refract(incident, h, etai / etat, refracted)) {
      srec.specular_ray = Ray(rec.p - n * 0.001f, refracted);
    } else {
      Vec3 reflected = tracer::optics::reflect(incident, h);
      srec.specular_ray = Ray(rec.p + n * 0.001f, reflected);
    }
  }

  return true;
}

Vec3 OceanMaterial::scattering_pdf(const Ray &r_in, const hit_record &rec,
                                   const Ray &scattered) const {
  return Vec3(0.0f, 0.0f, 0.0f);
}

} // namespace material
} // namespace tracer