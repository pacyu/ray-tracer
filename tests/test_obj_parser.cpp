#include "tracer/tracer.h"

using namespace tracer;

int main() {
  const int image_width = 1200;
  const int image_height = 600;
  const int samples_per_pixel = 128;
  const int max_depth = 8;
  Vec3 lookfrom(3.0f, 3.0f, 3.0f);
  Vec3 lookat(0.0f, 0.0f, 0.0f);
  Vec3 vup(0.0f, 0.0f, 1.0f);

  hittable_list world;
  hittable_list lights;

  std::shared_ptr<Background> background = std::make_shared<ImageBackground>(
      "../textures/autumn_field_puresky_4k.hdr");
  obj_parser::Object obj("../models/free-datsun-280z/source/Datsun_280Z.obj");

  std::vector<std::shared_ptr<geometry::Mesh>> meshes = obj.take();
  for (const auto &mesh : meshes) {
    mesh->finalize();
    world.add(mesh);
  }

  auto sun_mat =
      std::make_shared<material::DiffuseLight>(Vec3(15.0f, 15.0f, 15.0f));
  auto light = std::make_shared<geometry::Sphere>(Vec3(0.0f, 0.0f, 1000.0f),
                                                  30.0f, sun_mat);

  lights.add(light);

  BVH bvh(world);

  Camera camera(image_width, image_height, samples_per_pixel, max_depth,
                "test_obj_image.png", background, lookfrom, lookat, vup, 45.0f);
  camera.render(bvh, lights, false);
  return 0;
}