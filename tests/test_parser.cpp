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
  factory.builder();

  // 验证解析结果
  assert(factory.get_camera().image_width == 800);
  assert(factory.get_camera().image_height == 600);
  assert(factory.get_camera().samples_per_pixel == 128);
  assert(factory.get_camera().max_depth == 8);
  assert(factory.get_camera().background == Color(0.0, 0.0, 0.0));
}

int main() {
  test_parser();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}