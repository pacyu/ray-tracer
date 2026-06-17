#include "tracer/parser/factory.h"
#include "tracer/utils/timer.h"

using namespace tracer;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "错误: 未提供场景文件路径。" << std::endl;
    std::cerr << "使用方法: " << argv[0] << " <场景文件路径> [--heatmap]"
              << std::endl;
    return 1;
  }
  const std::string scene_path = argv[1];

  try {
    parser::Factory factory(scene_path);
    factory.parse();
    factory.builder();

    Camera cam = factory.take_camera();
    hittable_list lights = factory.take_lights();
    hittable_list world = factory.take_world();

    BVH bvh_world = BVH(world);

    {
      utils::RenderTimer timer("渲染");

      if (argc <= 2) {
        cam.render(bvh_world, lights, false);
      } else if (std::string(argv[2]) == "--heatmap") {
        std::cout << "当前渲染效果为热力图模式" << std::endl;
        cam.render(bvh_world, lights, true);
      }
    }
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