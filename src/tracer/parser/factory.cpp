#include "tracer/parser/factory.h"

namespace tracer {
namespace parser {

Factory::Factory(const std::string &filename) : lights(), world(), ast() {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  source = buffer.str();

  camera = std::make_unique<Camera>();
}

void Factory::parse() {
  Lexer lexer(source);
  auto tokens = lexer.tokenizer();

  Parser parser(tokens);
  parser.parse();
  ast = parser.get_ast();
}

void Factory::get_parameter(std::shared_ptr<Environment> &env,
                            const std::string &name) {
  try {
    BasicType val = env->get(name);
    if (name == "image_shape") {
      camera->image_width = val.t_pair.first->t_integer;
      camera->image_height = val.t_pair.second->t_integer;
    } else if (name == "spp") {
      camera->samples_per_pixel = val.t_integer;
    } else if (name == "depth") {
      camera->max_depth = val.t_integer;
    } else if (name == "background") {
      camera->background = std::make_shared<PhysicalSky>(val.t_vector3);
    } else if (name == "from") {

    } else if (name == "at") {

    } else if (name == "vup") {

    } else if (name == "fov") {

    } else if (name == "world") {
      for (const auto &item : val.t_list) {
        if (item.tag == BasicType::T_OBJECT) {
          world.add(item.t_object);
        }
      }
    } else if (name == "camera") {
      camera = std::move(val.t_camera);
    }
  } catch (...) {
    std::cerr << "Not found variable '" + name + "'!" << std::endl;
  }
}

void Factory::create_scene(std::shared_ptr<Environment> &env) {
  std::vector<std::string> params = {
      "image_shape", "spp", "depth", "background", "from",
      "at",          "vup", "fov",   "world",      "camera"};
  for (const std::string &param : params) {
    get_parameter(env, param);
  }
}

void Factory::builder() {
  std::shared_ptr<Environment> global_env = std::make_shared<Environment>();

  for (const auto &[name, node] : ast.program) {
    try {
      BasicType value = node->evaluate(global_env);
      global_env->set_environment(name, value);
    } catch (const std::exception &e) {
      std::cerr << "Evaluate Error [" << name << "]: " << e.what() << std::endl;
      return;
    }
  }

  create_scene(global_env);

  for (const auto &object : world.objects) {
    auto mat_ptr = object->get_material();
    if (mat_ptr && mat_ptr->is_emitter()) {
      lights.add(object);
    }
  }
}

} // namespace parser
} // namespace tracer