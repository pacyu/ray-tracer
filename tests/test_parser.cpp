#include "tracer/parser/ast.h" // 确保包含你的 AST 定义
#include "tracer/parser/factory.h"
#include "tracer/parser/lexer.h"
#include "tracer/parser/parser.h"
#include "tracer/tracer.h"
#include <assert.h>

using namespace tracer;
using namespace tracer::parser;

void test_parser() {
  // 使用工厂类解析场景描述
  Factory factory("..\\bin\\scene.aur");
  factory.parse();
  factory.builder();

  std::unique_ptr<Camera> camera = std::move(factory.get_camera());
  hittable_list world = factory.get_world();
  // 验证解析结果
  assert(camera->image_width == 600);
  assert(camera->image_height == 600);
  assert(camera->samples_per_pixel == 128);
  assert(camera->max_depth == 8);
  assert(camera->background == Color(0.0, 0.0, 0.0));
  assert(world.objects.size() >= 9);
}

int main() {
  test_parser();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}