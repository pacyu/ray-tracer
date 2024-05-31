#pragma once
#include <boost/math/tools/roots.hpp>
#include <boost/math/special_functions/next.hpp> // For float_distance.
#include <tuple> // for std::tuple and std::make_tuple.
#include <boost/math/special_functions/cbrt.hpp> // For boost::math::cbrt.

#include "hittable.h"

inline float square(float x) { return x * x; }
inline float cube(float x) { return x * x * x; }

inline float g(float x, float y, float z) {
  return (square(x) + 9.f / 80.f * square(y)) * cube(z);
}

inline float h(float x, float y, float z) {
  float a = square(x) + 9.f / 4.f * square(y) + square(z) - 1.f;
  return cube(a) - g(x, y, z);
}

inline float Fx(Point3 p, Point3 center) {
  return 2.f * (p.x() - center.x())
    * (3.f * square(square(p.x() - center.x()) + 9.f / 4.f * square(p.y() - center.y()) + square(p.z() - center.z()) - 1.f) - cube(p.z() - center.z()));
}

inline float Fy(Point3 p, Point3 center) {
  return 9.f / 2.f * (p.y() - center.y())
    * (3.f * square(square(p.x() - center.x()) + 9.f / 4.f * square(p.y() - center.y()) + square(p.z() - center.z()) - 1.f) - 1.f / 20.f * cube(p.z() - center.z()));
}

inline float Fz(Point3 p, Point3 center) {
  return 6.f * (p.z() - center.z())
    * square(square(p.x() - center.x()) + 9.f / 4.f * square(p.y() - center.y()) + square(p.z() - center.z()) - 1.f)
    - (3.f * square(p.x() - center.x()) + 27.f / 80.f * square(p.y() - center.y()))
    * square(p.z() - center.z());
}

template <class T>
struct cbrt_functor_noderiv
{
  //  cube root of x using only function - no derivatives.
  cbrt_functor_noderiv(T const& to_find_root_of, const Ray& r, const Point3& p)
    : a(to_find_root_of), r(r), p(p)
  { /* Constructor just stores value a to find root of. */
  }
  T operator()(T const& t)
  {
    //(r.origin() - p) + t * r.direction()).
    auto x = (r.origin().x() + t * r.direction().x()) - p.x();
    auto y = (r.origin().y() + t * r.direction().y()) - p.y();
    auto z = (r.origin().z() + t * r.direction().z()) - p.z();
    T ft = cube(square(x)) // x^6
      + 3.f * square(square(x)) * square(y) // + 3x^4 y^2
      + (27.f * square(square(x)) * square(z)) / 4.f // + 27/4 x^4 z^2
      - 3.f * square(square(x)) // -3x^4
      + 3.f * square(x) * square(square(y)) // + 3x^2 y^4
      - square(x) * cube(y) // - x^2 y^3
      + 27.f / 2.f * square(x) * square(z) * square(y) // + 27/2 x^2 y^2 z^2
      - 6.f * square(x) * square(y) // - 6 x^2 y^2
      + (243.f * square(x) * square(square(z))) / 16.f // + 243/16 x^2 z^4
      - (27.f * square(x) * square(z)) / 2.f // - 27/2 x^2 z^2
      + 3.f * square(x) // + 3 x^2
      + cube(square(y)) // + y^6
      + (27.f * square(z) * square(square(y))) / 4.f // + 27/4 y^4 z^2
      - 3.f * square(square(y)) // - 3 y^4
      - 9.f / 80.f * square(z) * cube(y) // - 9/80 y^3 z^2
      + (243.f * square(square(z)) * square(y)) / 16.f // + 243/16 y^2 z^4
      - (27.f * square(z) * square(y)) / 2.f // - 27/2 y^2 z^2
      + 3.f * square(y) // + 3 y^2
      + (729.f * cube(square(z))) / 64.f // + 729/64 z^6
      - (243.f * square(square(z))) / 16.f // - 243/16 z^4
      + (27.f * square(z)) / 4.f // + 27/4 z^2
      - a;
    return ft;
  }
private:
  T a;
  Ray r;
  Point3 p;
};

template <class T>
T cbrt_noderiv(T a, const Ray&r_in, const Point3& p)
{
  // return cube root of x using bracket_and_solve (no derivatives).
  using namespace boost::math::tools;           // For bracket_and_solve_root.

  int exponent;
  frexp(a, &exponent);                          // Get exponent of z (ignore mantissa).
  T guess = ldexp(1., exponent / 3);              // Rough guess is to divide the exponent by three.
  T factor = 1;                                 // How big steps to take when searching.

  const boost::uintmax_t maxit = 100;            // Limit to maximum iterations.
  boost::uintmax_t it = maxit;                  // Initally our chosen max iterations, but updated with actual.
  bool is_rising = true;                        // So if result if guess^3 is too low, then try increasing guess.
  int digits = std::numeric_limits<T>::digits;  // Maximum possible binary digits accuracy for type T.
  // Some fraction of digits is used to control how accurate to try to make the result.
  int get_digits = digits - 3;                  // We have to have a non-zero interval at each step, so
                                                // maximum accuracy is digits - 1.  But we also have to
                                                // allow for inaccuracy in f(x), otherwise the last few
                                                // iterations just thrash around.
  eps_tolerance<T> tol(get_digits);             // Set the tolerance.
  std::pair<T, T> r = bracket_and_solve_root(cbrt_functor_noderiv<T>(a, r_in, p), guess, factor, is_rising, tol, it);

  return r.first + (r.second - r.first) / 2;      // Midway between brackets is our result, if necessary we could
                                                // return the result as an interval here.
}

class Heart :public hittable {
public:
	Heart(Point3 center, float r, std::shared_ptr<Material> m)
		: center(center), rho(r), mat_ptr(m) {}

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, AABB& output_box) const;

	Point3 center;
  float rho;
	std::shared_ptr<Material> mat_ptr;
};

bool Heart::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
  float t;
  try {
    t = rho * cbrt_noderiv(1.f, r, center);
  }
  catch (...) {
    return false;
  }

  if (t < t_min || t > t_max) return false;
  std::cout << t << std::endl;
  rec.u = 0.f;
  rec.v = 0.f;
  rec.t = t;
	rec.p = r.at(t);
	rec.set_face_normal(r, unit_vector(Vec3(Fx(rec.p, center),
                                          Fy(rec.p, center),
                                          -1)));
	rec.mat_ptr = mat_ptr;
	return true;
}

bool Heart::bounding_box(float t0, float t1, AABB& output_box) const
{
    return true;
}