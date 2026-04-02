#pragma once
#include "tracer/math/math.h"
#include "tracer/math/vec3.h"

namespace tracer {
namespace optics {

Vec3 reflect(const Vec3 &v, const Vec3 &n);

Vec3 refract(const Vec3 &uv, const Vec3 &n, float etai_over_etat);

bool refract(const Vec3 &v, const Vec3 &n, float ni_over_nt, Vec3 &refracted);

float schlick(float cosine, float ref_idx);

float D_GGX(float NoH, float alpha);

float G_Smith(float NoV, float NoL, float alpha);

float fresnel_schlick(float cos_theta, float etai, float etat);

} // namespace optics
} // namespace tracer