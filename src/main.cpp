#include "tracer/parser/factory.h"
#include <cmath>
#include <cstdio>
#include <limits>

#pragma warning(disable : 4996)

using namespace tracer;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "错误: 未提供场景文件路径。" << std::endl;
    std::cerr << "使用方法: " << argv[0] << " <场景文件路径>" << std::endl;
    return 1;
  }
  const std::string scene_path = argv[1];

  try {
    parser::Factory factory(scene_path);
    factory.parse();
    factory.builder();

    std::unique_ptr<Camera> cam = std::move(factory.get_camera());
    auto light = factory.get_light();
    hittable_list world = factory.get_world();
    std::unique_ptr<hittable> bvh_world =
        std::make_unique<BVH>(world.objects, 0, world.objects.size());

    cam->render(*bvh_world, light);

  } catch (const parser::ParseException &e) {
    // 捕获带行号的自定义解析异常
    std::cerr << "\n[Auroric Parse Error] " << e.what() << std::endl;
    return 1;
  } catch (const std::runtime_error &e) {
    // 捕获通用的运行时错误（如文件打不开）
    std::cerr << "\n[Runtime Error] " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    // 捕获其他标准异常
    std::cerr << "\n[General Error] " << e.what() << std::endl;
    return 1;
  }

  return 0;
}