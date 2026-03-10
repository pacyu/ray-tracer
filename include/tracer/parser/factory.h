#pragma once
#include "tracer/parser/lexer.h"
#include "tracer/parser/parser.h"
#include "tracer/tracer.h"
#include <fstream>
#include <sstream>

namespace tracer {

namespace parser {

class Factory {
public:
  Factory(const std::string &filename);
  ~Factory() = default;

  void builder();

  const Camera &get_camera() const { return camera; }
  const std::shared_ptr<flip_face> &get_light() const { return light; }
  const hittable_list &get_world() const { return world; }

private:
  std::string source;

  Camera camera;
  std::shared_ptr<flip_face> light;
  hittable_list world;
};

} // namespace parser

} // namespace tracer