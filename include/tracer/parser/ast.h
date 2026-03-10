#pragma once

#include "tracer/core/vec3.h"
#include <string>
#include <variant>
#include <vector>

namespace tracer {

namespace parser {

struct pair {
  int x;
  int y;
};

struct quaternion {
  float x;
  float y;
  float z;
  float w;
};

struct ASTNode {
  virtual ~ASTNode() = default;
};

class PropertyNode : public ASTNode {
public:
  std::string name;
  std::variant<int, float, pair, Vec3, quaternion, std::string> value;
};

class ObjectNode : public ASTNode {
public:
  std::string type;
  std::vector<PropertyNode> properties;
};

class SceneAST {
public:
  std::vector<ObjectNode> objects;
};

} // namespace parser

} // namespace tracer