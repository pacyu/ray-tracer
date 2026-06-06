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

  const Camera &get_camera() const { return camera; }
  const hittable_list &get_lights() const { return lights; }
  const hittable_list &get_world() const { return world; }
  const Auroric &get_ast() const { return ast; }
  Camera take_camera() { return std::move(camera); }
  hittable_list take_lights() { return std::move(lights); }
  hittable_list take_world() { return std::move(world); }
  Auroric take_ast() { return std::move(ast); }

private:
  std::string source;
  Auroric ast;
  Camera camera;
  hittable_list world;
  hittable_list lights;

  void get_parameter(std::shared_ptr<Environment> &, const std::string &);
  void create_scene(std::shared_ptr<Environment> &);
};

} // namespace parser
} // namespace tracer