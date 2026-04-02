#pragma once
#include "tracer/math/drand48.h"
#include "tracer/math/math.h"
#include "tracer/math/vec3.h"

namespace tracer {
namespace math {

Vec3 random_cosine_direction();

Vec3 random_unit_vector();

Vec3 random_to_sphere(float radius, float distance_squared);

Vec3 random_in_unit_sphere();

Vec3 random_triangle_barycentric();

Vec3 sample_HG(const Vec3 &wo, float g);

Vec3 sample_GGX(float alpha);

} // namespace math
} // namespace tracer