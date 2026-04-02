#pragma once
#include "tracer/core/camera.h"
#include "tracer/core/hittable.h"
#include "tracer/geometry/aarect.h"
#include "tracer/geometry/box.h"
#include "tracer/geometry/heart.h"
#include "tracer/geometry/ocean.h"
#include "tracer/geometry/sphere.h"
#include "tracer/material/dielectric.h"
#include "tracer/material/diffuse_light.h"
#include "tracer/material/lambertian.h"
#include "tracer/material/metal.h"
#include "tracer/material/ocean_material.h"
#include "tracer/math/vec3.h"
#include "tracer/texture/image_texture.h"
#include "tracer/transform/rotate.h"
#include "tracer/transform/translate.h"
#include "tracer/volume/constant_medium.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace tracer {
namespace parser {

struct Environment;
struct ASTNode;

struct Quaternion {
  float x;
  float y;
  float z;
  float w;
  Quaternion() : x(0), y(0), z(0), w(0) {}
  Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct Lambda {
  std::string param_name;
  std::shared_ptr<ASTNode> body;
  std::shared_ptr<Environment> closure_env; // 闭包环境
};

struct BasicType {
  BasicType() : tag() {}

  BasicType(int val) : t_integer(val), tag(T_INT) {}

  BasicType(int r_min, int r_max)
      : t_integer(tracer::math::random_int(r_min, r_max)), tag(T_RANDOM) {}

  BasicType(float r_min, float r_max)
      : t_float(tracer::math::random_float(r_min, r_max)), tag(T_RANDOM) {}

  BasicType(float val) : t_float(val), tag(T_FLOAT) {}

  BasicType(
      std::pair<std::shared_ptr<BasicType>, std::shared_ptr<BasicType>> val)
      : t_pair(std::move(val)), tag(T_PAIR) {}

  BasicType(Vec3 val) : t_vector3(val), tag(T_VEC3) {}

  BasicType(Quaternion val) : t_quater(val), tag(T_QUATER) {}

  BasicType(std::vector<BasicType> val) : t_list(std::move(val)), tag(T_LIST) {}

  BasicType(std::string val) : t_string(std::move(val)), tag(T_STRING) {}

  BasicType(Lambda val) : t_lambda(std::move(val)), tag(T_LAMBDA) {}

  BasicType(std::shared_ptr<Material> val)
      : t_material(std::move(val)), tag(T_MATERIAL) {}

  BasicType(std::shared_ptr<hittable> val)
      : t_object(std::move(val)), tag(T_OBJECT) {}

  BasicType(std::unique_ptr<Camera> val)
      : t_camera(std::move(val)), tag(T_CAMERA) {}

  BasicType(const BasicType &other)
      : tag(other.tag), t_list(other.t_list), t_lambda(other.t_lambda),
        t_integer(other.t_integer), t_float(other.t_float),
        t_string(other.t_string), t_pair(other.t_pair),
        t_vector3(other.t_vector3), t_quater(other.t_quater),
        t_material(other.t_material), t_object(other.t_object) {}

  BasicType &operator=(const BasicType &other) {
    if (this == &other)
      return *this;
    tag = other.tag;
    t_list = other.t_list;
    t_lambda = other.t_lambda;
    t_integer = other.t_integer;
    t_float = other.t_float;
    t_string = other.t_string;
    t_pair = other.t_pair;
    t_vector3 = other.t_vector3;
    t_quater = other.t_quater;
    t_material = other.t_material;
    t_object = other.t_object;
    return *this;
  }

  BasicType(BasicType &&other) noexcept
      : tag(other.tag), t_list(std::move(other.t_list)),
        t_lambda(std::move(other.t_lambda)), t_integer(other.t_integer),
        t_float(other.t_float), t_string(std::move(other.t_string)),
        t_pair(std::move(other.t_pair)), t_vector3(other.t_vector3),
        t_quater(other.t_quater), t_material(std::move(other.t_material)),
        t_object(std::move(other.t_object)),
        t_camera(std::move(other.t_camera)) {}

  enum {
    T_INT,
    T_FLOAT,
    T_PAIR,
    T_VEC3,
    T_QUATER,
    T_LIST,
    T_STRING,
    T_LAMBDA,
    T_RANDOM,
    T_MATERIAL,
    T_OBJECT,
    T_CAMERA
  } tag;

  std::vector<BasicType> t_list;

  Lambda t_lambda;

  int t_integer = 0;
  float t_float = 0.0f;
  std::string t_string;
  std::pair<std::shared_ptr<BasicType>, std::shared_ptr<BasicType>> t_pair;
  Vec3 t_vector3;
  Quaternion t_quater;
  std::shared_ptr<Material> t_material;
  std::shared_ptr<hittable> t_object;
  std::unique_ptr<Camera> t_camera;
};

struct ASTNode {
  virtual ~ASTNode() = default;
  virtual BasicType evaluate(std::shared_ptr<Environment> env) = 0;
};

struct Statement {
  std::string name;
  std::shared_ptr<ASTNode> node;

  Statement() {}
  Statement(std::string, std::shared_ptr<ASTNode>);
};

class Auroric {
public:
  std::vector<Statement> program;
};

struct Environment : public std::enable_shared_from_this<Environment> {
  std::unordered_map<std::string, BasicType> values;
  std::shared_ptr<Environment> outer;

  Environment() {}
  Environment(std::shared_ptr<Environment> p) : outer(std::move(p)) {}

  void set_environment(const std::string &, BasicType);
  BasicType get(const std::string &);
};

class LambdaNode : public ASTNode {
private:
  std::string param_name;
  std::shared_ptr<ASTNode> body;

public:
  LambdaNode(std::string, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class PipeNode : public ASTNode {
private:
  char op;
  std::shared_ptr<ASTNode> left, right;

public:
  PipeNode(char, std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);
  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class ListNode : public ASTNode {
private:
  enum Mode { ENUMERATED, RANGE } mode;
  std::vector<std::shared_ptr<ASTNode>> elements;
  std::shared_ptr<ASTNode> start, end, step;

public:
  ListNode(std::vector<std::shared_ptr<ASTNode>>);
  ListNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);
  ListNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
           std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class BinaryOpNode : public ASTNode {
private:
  char op; // '+', '-', '*', '/', '%'
  std::shared_ptr<ASTNode> left, right;

public:
  BinaryOpNode(char, std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class UnaryOpNode : public ASTNode {
private:
  char op; // '-'
  std::shared_ptr<ASTNode> expr;

public:
  UnaryOpNode(char, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class LiteralNode : public ASTNode {
private:
  BasicType value;

public:
  LiteralNode(BasicType);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class VariableNode : public ASTNode {
private:
  std::string name;

public:
  VariableNode(std::string);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class PairNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> fst_expr, snd_expr;

public:
  PairNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class Vec3Node : public ASTNode {
private:
  std::shared_ptr<ASTNode> x_expr, y_expr, z_expr;

public:
  Vec3Node(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
           std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class QuaterNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> x_expr, y_expr, z_expr, w_expr;

public:
  QuaterNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class RandomNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> fst_expr, snd_expr;

public:
  RandomNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class LambertianNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> albedo_expr;

public:
  LambertianNode(std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class MetalNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> albedo_expr, fuzz_expr;

public:
  MetalNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class DielectricNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> albedo_expr, density_expr, ri_expr;

public:
  DielectricNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
                 std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class OceanMaterialNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> ior_expr, roughness_expr;

public:
  OceanMaterialNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class DiffuseLightNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> albedo_expr;

public:
  DiffuseLightNode(std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class XYRectNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> material_expr, quater_expr, k_expr;

public:
  XYRectNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class XZRectNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> material_expr, quater_expr, k_expr;

public:
  XZRectNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class YZRectNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> material_expr, quater_expr, k_expr;

public:
  YZRectNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class BoxNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> material_expr, min_expr, max_expr, rot_expr;

public:
  BoxNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
          std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class SphereNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> center_expr, radius_expr, material_expr;

public:
  SphereNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class HeartNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> center_expr, rho_expr, material_expr, rot_expr;

public:
  HeartNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
            std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class OceanNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> n_expr, l_expr, speed_expr, dir_expr, a_expr,
      mat_expr, lambda_expr, height_expr;

public:
  OceanNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
            std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
            std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
            std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class TranslateNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> object_expr, offset_expr;

public:
  TranslateNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class ConstantMediumNode : public ASTNode {
private:
  std::shared_ptr<ASTNode> albedo_expr, density_expr, object_expr;

public:
  ConstantMediumNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
                     std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

class CameraNode : public ASTNode {
public:
  std::shared_ptr<ASTNode> shape_expr, spp_expr, depth_expr, name_expr, bg_expr,
      from_expr, at_expr, vup_expr, fov_expr;
  CameraNode() {};
  CameraNode(std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
             std::shared_ptr<ASTNode>);

  virtual BasicType evaluate(std::shared_ptr<Environment> env) override;
};

} // namespace parser
} // namespace tracer