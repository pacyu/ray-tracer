#include "tracer/core/box.h"

namespace tracer {

bool Box::hit(const Ray &r, float t0, float t1, hit_record &rec) const {
  return sides.hit(r, t0, t1, rec);
}

} // namespace tracer