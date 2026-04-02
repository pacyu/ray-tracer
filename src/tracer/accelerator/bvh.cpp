#include "tracer/accelerator/bvh.h"

namespace tracer {

BVH::BVH(std::vector<std::shared_ptr<hittable>> &objects, size_t start,
         size_t end) {
  int axis = math::random_int(0, 2);

  auto comparator = (axis == 0)   ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

  size_t object_span = end - start;

  if (object_span == 1) {
    left = right = objects[start];
  } else if (object_span == 2) {
    if (comparator(objects[start], objects[start + 1])) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      left = objects[start + 1];
      right = objects[start];
    }
  } else {
    std::sort(std::begin(objects) + start, std::begin(objects) + end,
              comparator);

    auto mid = start + object_span / 2;
    left = std::make_unique<BVH>(objects, start, mid);
    right = std::make_unique<BVH>(objects, mid, end);
  }

  AABB box_left, box_right;
  if (!left->bounding_box(0, 0, box_left) ||
      !right->bounding_box(0, 0, box_right))
    std::cerr << "No bounding box in BVH constructor.\n";
  bbox = AABB::surrounding_box(box_left, box_right);
}

bool BVH::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
  if (!bbox.hit(r, t_min, t_max))
    return false;

  r.bvh_hit_count++;

  bool hit_left = left->hit(r, t_min, t_max, rec);
  bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

  return hit_left || hit_right;
}

bool BVH::bounding_box(float t0, float t1, AABB &output_box) const {
  output_box = bbox;
  return true;
}

void BVH::refit(float t0, float t1) {
  if (left)
    left->refit(t0, t1);
  if (right)
    right->refit(t0, t1);
  AABB box_left, box_right;
  bool has_left = left && left->bounding_box(t0, t1, box_left);
  bool has_right = right && right->bounding_box(t0, t1, box_right);

  // 3. 重新拟合当前节点的 box
  if (!has_left && !has_right) {
    // 如果两边都没包围盒，这个节点理论上应该失效
  } else if (!has_left) {
    bbox = box_right;
  } else if (!has_right) {
    bbox = box_left;
  } else {
    bbox = AABB::surrounding_box(box_left, box_right);
  }
}

std::shared_ptr<Material> BVH::get_material() const { return nullptr; }

} // namespace tracer