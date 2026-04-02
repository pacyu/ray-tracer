#include "tracer/parser/ast.h"
#include "tracer/parser/factory.h"
#include "tracer/parser/lexer.h"
#include "tracer/parser/parser.h"
#include "tracer/tracer.h"
#include <assert.h>

using namespace tracer;
using namespace tracer::parser;

static void test_parser() {
  // 使用工厂类解析场景 aur 文件
  Factory factory("..\\bin\\scene.aur");
  factory.parse();
  factory.builder();

  std::unique_ptr<Camera> camera = std::move(factory.get_camera());
  hittable_list world = factory.get_world();

  std::shared_ptr<Environment> global_env = std::make_shared<Environment>();
  for (const auto &[name, node] : factory.get_ast().program) {
    BasicType value = node->evaluate(global_env);
    global_env->set_environment(name, value);
    if (value.tag == BasicType::T_OBJECT) {
      std::cout << name << ": " << value.t_object << std::endl;
    } else if (value.tag == BasicType::T_MATERIAL) {
      std::cout << name << ": " << value.t_material << std::endl;
    } else if (value.tag == BasicType::T_LIST) {
      std::cout << name << ": ";
      size_t length = value.t_list.size();
      for (size_t i = 0; i < length; i++) {
        BasicType item = value.t_list[i];
        if (item.tag == BasicType::T_INT)
          std::cout << item.t_integer;
        else if (item.tag == BasicType::T_FLOAT)
          std::cout << item.t_float;
        else if (item.tag == BasicType::T_VEC3)
          std::cout << item.t_vector3;
        else if (item.tag == BasicType::T_OBJECT)
          std::cout << item.t_object;
        else if (item.tag == BasicType::T_RANDOM)
          std::cout << item.t_integer;

        if (i < length - 1) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
    } else if (value.tag == BasicType::T_STRING) {
      std::cout << name << ": " << value.t_string << std::endl;
    } else if (value.tag == BasicType::T_RANDOM) {
      std::cout << name << ": " << value.t_integer << std::endl;
    } else if (value.tag == BasicType::T_INT) {
      std::cout << name << ": " << value.t_integer << std::endl;
    } else if (value.tag == BasicType::T_FLOAT) {
      std::cout << name << ": " << value.t_float << std::endl;
    } else if (value.tag == BasicType::T_LAMBDA) {
      std::cout << name << ": " << value.t_lambda.param_name << " -> "
                << value.t_lambda.body << std::endl;
    }
  }

  // 验证解析结果
  assert(camera->image_width == 600);
  assert(camera->image_height == 600);
  assert(camera->samples_per_pixel >= 128);
  assert(camera->max_depth == 8);
  assert(world.objects.size() >= 9);
}

int main() {
  test_parser();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}