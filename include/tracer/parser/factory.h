#pragma once
#include "tracer/parser/lexer.h"
#include "tracer/parser/parser.h"
#include "tracer/tracer.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace tracer {

namespace parser {

class Factory {
public:
  Factory(const std::string &filename);
  ~Factory() = default;

  void builder();
  void parse();

  std::unique_ptr<Camera> &get_camera() { return camera; }
  const hittable_list &get_lights() const { return lights; }
  const hittable_list &get_world() const { return world; }
  const Auroric &get_ast() const { return ast; }

private:
  std::string source;
  Auroric ast;
  std::unique_ptr<Camera> camera;
  hittable_list world;
  hittable_list lights;

  void get_parameter(std::shared_ptr<Environment> &, const std::string &);
  void create_scene(std::shared_ptr<Environment> &);
};

} // namespace parser

} // namespace tracer