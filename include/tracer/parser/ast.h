#pragma once

#include "tracer/core/hittable.h"
#include "tracer/core/material.h"
#include "tracer/core/vec3.h"
#include <string>
#include <variant>
#include <vector>

namespace tracer {

namespace parser {

struct Pair {
  int x;
  int y;
};

struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};

struct ASTNode {
  virtual ~ASTNode() = default;
};

class Argument : public ASTNode {
public:
  enum { INT, FLOAT, PAIR, VECTOR3, QUATERNION, STRING } type;
  std::string name;
  std::variant<int, float, Pair, Vec3, Quaternion, std::string> value;
};

class InstanceNode : public ASTNode {
public:
  enum { NAMED, ANONYMOUS, EXPRESSION } kind;
  std::string _typename;
  std::string varname;
  Argument arg;
  std::vector<Argument> args;
};

class SceneAST {
public:
  std::vector<InstanceNode> instances;
};

class Instance {
public:
  enum {
    INT,
    FLOAT,
    STRING,
    PAIR,
    VECTOR3,
    QUATERNION,
    CAMERA,
    LAMBERTIAN,
    METAL,
    DIELECTRIC,
    RECT,
    BOX,
    HEART,
    SPHERE,
    CONSTANT_MEDIUM
  } kind;
  std::string _typename;
  std::string name;
  Argument arg;
  std::vector<Argument> args;
};

struct BasicType {
  BasicType() = default;
  virtual ~BasicType() = default;

  int t_integer = 0;
  float t_float = 0.0f;
  Pair t_pair;
  Vec3 t_vector3;
  Quaternion t_quat;
  std::string t_string;
  std::shared_ptr<Material> t_material;
  std::shared_ptr<hittable> t_object;
};

} // namespace parser

} // namespace tracer