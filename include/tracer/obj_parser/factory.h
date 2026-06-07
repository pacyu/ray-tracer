#pragma once
#include "tracer/material/standard_material.h"
#include "tracer/texture/image_texture.h"
#include "tracer/obj_parser/ast.h"
#include "tracer/obj_parser/lexer.h"
#include "tracer/obj_parser/parser.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace tracer {
namespace obj_parser {

class Object {
public:
  Object() {}
  Object(const std::string &);

  std::shared_ptr<geometry::Mesh> take();

private:
  std::string source;
  ObjParser o_parser;
  ObjParser mtl_parser;
  std::shared_ptr<geometry::Mesh> mesh;

  void builder();
};

} // namespace obj_parser
} // namespace tracer