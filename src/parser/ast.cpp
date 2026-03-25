#include "tracer/parser/ast.h"

namespace tracer {

namespace parser {

Statement::Statement(std::string name, std::shared_ptr<ASTNode> node)
    : name(std::move(name)), node(std::move(node)) {}

void Environment::set_environment(const std::string &name, BasicType val) {
  values[name] = std::move(val);
}

BasicType Environment::get(const std::string &name) {
  if (values.count(name))
    return values[name];
  if (outer)
    return outer->get(name);
  throw std::runtime_error("Undefined variable: " + name);
}

LambdaNode::LambdaNode(std::string name, std::shared_ptr<ASTNode> body)
    : param_name(std::move(name)), body(std::move(body)) {}

PipeNode::PipeNode(char ch, std::shared_ptr<ASTNode> l,
                   std::shared_ptr<ASTNode> r)
    : op(ch), left(std::move(l)), right(std::move(r)) {}

ListNode::ListNode(std::vector<std::shared_ptr<ASTNode>> elements)
    : elements(std::move(elements)), mode(Mode::ENUMERATED) {}

ListNode::ListNode(std::shared_ptr<ASTNode> s, std::shared_ptr<ASTNode> e)
    : start(std::move(s)), end(std::move(e)), mode(Mode::RANGE) {}

ListNode::ListNode(std::shared_ptr<ASTNode> s, std::shared_ptr<ASTNode> e,
                   std::shared_ptr<ASTNode> p)
    : start(std::move(s)), end(std::move(e)), step(std::move(p)),
      mode(Mode::RANGE) {}

BinaryOpNode::BinaryOpNode(char ch, std::shared_ptr<ASTNode> l,
                           std::shared_ptr<ASTNode> r)
    : op(ch), left(std::move(l)), right(std::move(r)) {}

UnaryOpNode::UnaryOpNode(char ch, std::shared_ptr<ASTNode> expr)
    : op(ch), expr(std::move(expr)) {}

LiteralNode::LiteralNode(BasicType val) : value(std::move(val)) {}

VariableNode::VariableNode(std::string name) : name(std::move(name)) {}

PairNode::PairNode(std::shared_ptr<ASTNode> fst, std::shared_ptr<ASTNode> snd)
    : fst_expr(std::move(fst)), snd_expr(std::move(snd)) {}

Vec3Node::Vec3Node(std::shared_ptr<ASTNode> x, std::shared_ptr<ASTNode> y,
                   std::shared_ptr<ASTNode> z)
    : x_expr(std::move(x)), y_expr(std::move(y)), z_expr(std::move(z)) {}

QuaterNode::QuaterNode(std::shared_ptr<ASTNode> x, std::shared_ptr<ASTNode> y,
                       std::shared_ptr<ASTNode> z, std::shared_ptr<ASTNode> w)
    : x_expr(std::move(x)), y_expr(std::move(y)), z_expr(std::move(z)),
      w_expr(std::move(w)) {}

RandomNode::RandomNode(std::shared_ptr<ASTNode> first,
                       std::shared_ptr<ASTNode> second)
    : fst_expr(std::move(first)), snd_expr(std::move(second)) {}

LambertianNode::LambertianNode(std::shared_ptr<ASTNode> albedo)
    : albedo_expr(std::move(albedo)) {}

MetalNode::MetalNode(std::shared_ptr<ASTNode> albedo,
                     std::shared_ptr<ASTNode> fuzz)
    : albedo_expr(std::move(albedo)), fuzz_expr(std::move(fuzz)) {}

DielectricNode::DielectricNode(std::shared_ptr<ASTNode> albedo,
                               std::shared_ptr<ASTNode> density,
                               std::shared_ptr<ASTNode> ri)
    : albedo_expr(std::move(albedo)), ri_expr(std::move(ri)),
      density_expr(std::move(density)) {}

DiffuseLightNode::DiffuseLightNode(std::shared_ptr<ASTNode> albedo)
    : albedo_expr(std::move(albedo)) {}

XYRectNode::XYRectNode(std::shared_ptr<ASTNode> quater,
                       std::shared_ptr<ASTNode> k, std::shared_ptr<ASTNode> mat)
    : material_expr(std::move(mat)), quater_expr(std::move(quater)),
      k_expr(std::move(k)) {}

XZRectNode::XZRectNode(std::shared_ptr<ASTNode> quater,
                       std::shared_ptr<ASTNode> k, std::shared_ptr<ASTNode> mat)
    : material_expr(std::move(mat)), quater_expr(std::move(quater)),
      k_expr(std::move(k)) {}

YZRectNode::YZRectNode(std::shared_ptr<ASTNode> quater,
                       std::shared_ptr<ASTNode> k, std::shared_ptr<ASTNode> mat)
    : material_expr(std::move(mat)), quater_expr(std::move(quater)),
      k_expr(std::move(k)) {}

BoxNode::BoxNode(std::shared_ptr<ASTNode> bmin, std::shared_ptr<ASTNode> bmax,
                 std::shared_ptr<ASTNode> mat, std::shared_ptr<ASTNode> rot)
    : min_expr(std::move(bmin)), max_expr(std::move(bmax)),
      material_expr(std::move(mat)), rot_expr(std::move(rot)) {}

SphereNode::SphereNode(std::shared_ptr<ASTNode> center,
                       std::shared_ptr<ASTNode> radius,
                       std::shared_ptr<ASTNode> mat)
    : center_expr(std::move(center)), radius_expr(std::move(radius)),
      material_expr(std::move(mat)) {}

HeartNode::HeartNode(std::shared_ptr<ASTNode> center,
                     std::shared_ptr<ASTNode> rho, std::shared_ptr<ASTNode> mat,
                     std::shared_ptr<ASTNode> rot)
    : center_expr(std::move(center)), rho_expr(std::move(rho)),
      material_expr(std::move(mat)), rot_expr(std::move(rot)) {}

TranslateNode::TranslateNode(std::shared_ptr<ASTNode> object,
                             std::shared_ptr<ASTNode> offset)
    : object_expr(std::move(object)), offset_expr(std::move(offset)) {}

ConstantMediumNode::ConstantMediumNode(std::shared_ptr<ASTNode> albedo,
                                       std::shared_ptr<ASTNode> density,
                                       std::shared_ptr<ASTNode> object)
    : object_expr(std::move(object)), density_expr(std::move(density)),
      albedo_expr(std::move(albedo)) {}

CameraNode::CameraNode(
    std::shared_ptr<ASTNode> shape, std::shared_ptr<ASTNode> spp,
    std::shared_ptr<ASTNode> depth, std::shared_ptr<ASTNode> bg,
    std::shared_ptr<ASTNode> from, std::shared_ptr<ASTNode> at,
    std::shared_ptr<ASTNode> vup, std::shared_ptr<ASTNode> fov)
    : shape_expr(std::move(shape)), spp_expr(std::move(spp)),
      depth_expr(std::move(depth)), bg_expr(std::move(bg)),
      from_expr(std::move(from)), at_expr(std::move(at)),
      vup_expr(std::move(vup)), fov_expr(std::move(fov)) {}

BasicType LambdaNode::evaluate(std::shared_ptr<Environment> env) {
  // 返回一个闭包：包含函数体和当前的环境
  BasicType res;
  res.tag = BasicType::T_LAMBDA;
  res.t_lambda = {param_name, body, env};
  return res;
}

BasicType PipeNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType func = left->evaluate(env);
  BasicType argument = right->evaluate(env);

  if (func.tag == BasicType::T_LAMBDA) {
    std::shared_ptr<Environment> temp_env =
        std::make_shared<Environment>(func.t_lambda.closure_env);
    temp_env->set_environment(func.t_lambda.param_name, argument);
    return func.t_lambda.body->evaluate(temp_env);
  }
  throw std::runtime_error(
      "Left side of '|' must be a lambda expression or a .. expression.");
}

BasicType ListNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType value;
  value.tag = BasicType::T_LIST;

  if (mode == Mode::ENUMERATED) {
    for (auto &element : elements) {
      value.t_list.push_back(element->evaluate(env));
    }
  } else if (mode == Mode::RANGE) {
    BasicType _start = start->evaluate(env);
    BasicType _end = end->evaluate(env);
    BasicType _step = step->evaluate(env);
    if (_start.tag == BasicType::T_FLOAT) {
      float s = _start.t_float;
      float e = _end.t_float;
      float p = step ? _step.t_float : 1.0f;
      for (float i = s; i < e; i += p)
        value.t_list.push_back(BasicType(i));
    } else if (_start.tag == BasicType::T_INT) {
      int s = _start.t_integer;
      int e = _end.t_integer;
      int p = step ? _step.t_integer : 1;
      for (int i = s; i < e; i += p)
        value.t_list.push_back(BasicType(i));
    }
  }
  return value;
}

BasicType BinaryOpNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType l = left->evaluate(env);
  BasicType r = right->evaluate(env);
  if (op == '+') {
    if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_FLOAT) {
      return l.t_float + r.t_float;
    } else if (l.tag == BasicType::T_INT && r.tag == BasicType::T_INT) {
      return l.t_integer + r.t_integer;
    } else if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_INT) {
      return l.t_float + r.t_integer;
    } else {
      return l.t_integer + r.t_float;
    }
  } else if (op == '-') {
    if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_FLOAT) {
      return l.t_float - r.t_float;
    } else if (l.tag == BasicType::T_INT && r.tag == BasicType::T_INT) {
      return l.t_integer - r.t_integer;
    } else if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_INT) {
      return l.t_float - r.t_integer;
    } else {
      return l.t_integer - r.t_float;
    }
  } else if (op == '*') {
    if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_FLOAT) {
      return l.t_float * r.t_float;
    } else if (l.tag == BasicType::T_INT && r.tag == BasicType::T_INT) {
      return l.t_integer * r.t_integer;
    } else if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_INT) {
      return l.t_float * r.t_integer;
    } else {
      return l.t_integer * r.t_float;
    }
  } else if (op == '/') {
    if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_FLOAT) {
      return l.t_float / r.t_float;
    } else if (l.tag == BasicType::T_INT && r.tag == BasicType::T_INT) {
      return static_cast<float>(l.t_integer) / static_cast<float>(r.t_integer);
    } else if (l.tag == BasicType::T_FLOAT && r.tag == BasicType::T_INT) {
      return l.t_float / r.t_integer;
    } else {
      return l.t_integer / r.t_float;
    }
  } else if (op == '%') {
    if (l.tag == BasicType::T_FLOAT || r.tag == BasicType::T_FLOAT)
      throw std::runtime_error(
          std::string("Invalid operands to binary expression ('") +
          ((l.tag == BasicType::T_FLOAT) ? "float" : "int") +
          std::string("' and '") +
          ((r.tag == BasicType::T_FLOAT) ? "float" : "int") +
          std::string("'): '%' requires integer types."));
    return l.t_integer % r.t_integer;
  }
  throw std::runtime_error("Unknown operator '" + std::to_string(op) + "'.");
}

BasicType UnaryOpNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType v = expr->evaluate(env);
  if (op == '-') {
    if (v.tag == BasicType::T_FLOAT)
      return BasicType(-v.t_float);
    else
      return BasicType(-v.t_integer);
  }
  return v;
}

BasicType LiteralNode::evaluate(std::shared_ptr<Environment> env) {
  return value;
}

BasicType VariableNode::evaluate(std::shared_ptr<Environment> env) {
  return env->get(name);
}

BasicType PairNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType fst = fst_expr->evaluate(env);
  BasicType snd = snd_expr->evaluate(env);
  return BasicType(
      std::pair<std::shared_ptr<BasicType>, std::shared_ptr<BasicType>>(
          std::make_shared<BasicType>(fst), std::make_shared<BasicType>(snd)));
}

BasicType Vec3Node::evaluate(std::shared_ptr<Environment> env) {
  BasicType x = x_expr->evaluate(env);
  BasicType y = y_expr->evaluate(env);
  BasicType z = z_expr->evaluate(env);
  if (x.tag == BasicType::T_INT)
    x.t_float = static_cast<float>(x.t_integer);
  if (y.tag == BasicType::T_INT)
    y.t_float = static_cast<float>(y.t_integer);
  if (z.tag == BasicType::T_INT)
    z.t_float = static_cast<float>(z.t_integer);
  return BasicType(Vec3(x.t_float, y.t_float, z.t_float));
}

BasicType QuaterNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType x = x_expr->evaluate(env);
  BasicType y = y_expr->evaluate(env);
  BasicType z = z_expr->evaluate(env);
  BasicType w = w_expr->evaluate(env);
  if (x.tag == BasicType::T_INT)
    x.t_float = static_cast<float>(x.t_integer);
  if (y.tag == BasicType::T_INT)
    y.t_float = static_cast<float>(y.t_integer);
  if (z.tag == BasicType::T_INT)
    z.t_float = static_cast<float>(z.t_integer);
  if (w.tag == BasicType::T_INT)
    w.t_float = static_cast<float>(w.t_integer);
  return BasicType(Quaternion(x.t_float, y.t_float, z.t_float, w.t_float));
}

BasicType RandomNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType fst = fst_expr->evaluate(env);
  BasicType snd = snd_expr->evaluate(env);
  if (fst.tag == BasicType::T_INT && snd.tag == BasicType::T_INT)
    return BasicType(fst.t_integer, snd.t_integer);
  else if (fst.tag == BasicType::T_FLOAT && snd.tag == BasicType::T_FLOAT)
    return BasicType(fst.t_float, snd.t_float);
  throw std::runtime_error("Error type of 'Random' constructor paramaters.");
}

BasicType LambertianNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType albedo = albedo_expr->evaluate(env);
  if (albedo.tag == BasicType::T_VEC3) {
    return BasicType(std::make_shared<Lambertian>(albedo.t_vector3));
  } else {
    return BasicType(std::make_shared<Lambertian>(
        std::make_shared<ImageTexture>(albedo.t_string.c_str())));
  }
}

BasicType MetalNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType albedo = albedo_expr->evaluate(env);
  BasicType fuzz = fuzz_expr->evaluate(env);
  if (fuzz.tag == BasicType::T_INT)
    fuzz.t_float = static_cast<float>(fuzz.t_integer);
  return BasicType(std::make_shared<Metal>(albedo.t_vector3, fuzz.t_float));
}

BasicType DielectricNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType albedo = albedo_expr->evaluate(env);
  BasicType ni_density = density_expr->evaluate(env);
  BasicType ri = ri_expr->evaluate(env);
  if (ni_density.tag == BasicType::T_INT)
    ni_density.t_float = static_cast<float>(ni_density.t_integer);
  if (ri.tag == BasicType::T_INT)
    ri.t_float = static_cast<float>(ri.t_integer);
  return BasicType(std::make_shared<Dielectric>(
      albedo.t_vector3, ni_density.t_float, ri.t_float));
}

BasicType DiffuseLightNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType albedo = albedo_expr->evaluate(env);
  return BasicType(std::make_shared<DiffuseLight>(albedo.t_vector3));
}

BasicType XYRectNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType quater = quater_expr->evaluate(env);
  BasicType k_val = k_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  float x = quater.t_quater.x, y = quater.t_quater.y, z = quater.t_quater.z,
        w = quater.t_quater.w, k = k_val.t_float;
  return BasicType(std::make_shared<XYRect>(x, y, z, w, k, mat.t_material));
}

BasicType XZRectNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType quater = quater_expr->evaluate(env);
  BasicType k_val = k_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  float x = quater.t_quater.x, y = quater.t_quater.y, z = quater.t_quater.z,
        w = quater.t_quater.w, k = k_val.t_float;
  return BasicType(std::make_shared<XZRect>(x, y, z, w, k, mat.t_material));
}

BasicType YZRectNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType quater = quater_expr->evaluate(env);
  BasicType k_val = k_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  float x = quater.t_quater.x, y = quater.t_quater.y, z = quater.t_quater.z,
        w = quater.t_quater.w, k = k_val.t_float;
  return BasicType(std::make_shared<YZRect>(x, y, z, w, k, mat.t_material));
}

BasicType BoxNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType bmin = min_expr->evaluate(env);
  BasicType bmax = max_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  Vec3 dist = bmax.t_vector3 - bmin.t_vector3;
  std::shared_ptr<hittable> box =
      std::make_shared<Box>(Vec3(0, 0, 0), dist, mat.t_material);
  if (rot_expr) {
    BasicType rot = rot_expr->evaluate(env);
    float x = rot.t_vector3.x();
    float y = rot.t_vector3.y();
    float z = rot.t_vector3.z();
    if (x != 0.f)
      box = std::make_shared<RotateX>(box, x);
    if (y != 0.f)
      box = std::make_shared<RotateY>(box, y);
    if (z != 0.f)
      box = std::make_shared<RotateZ>(box, z);
  }
  box = std::make_shared<Translate>(box, bmin.t_vector3);
  return BasicType(box);
}

BasicType SphereNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType center = center_expr->evaluate(env);
  BasicType radius = radius_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  std::shared_ptr<hittable> sphere = std::make_shared<Sphere>(
      center.t_vector3, radius.t_float, mat.t_material);
  return BasicType(sphere);
}

BasicType HeartNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType center = center_expr->evaluate(env);
  BasicType rho = rho_expr->evaluate(env);
  BasicType mat = material_expr->evaluate(env);
  std::shared_ptr<hittable> heart =
      std::make_shared<Heart>(Vec3(0, 0, 0), rho.t_float, mat.t_material);
  if (rot_expr) {
    BasicType rot = rot_expr->evaluate(env);
    float x = rot.t_vector3.x();
    float y = rot.t_vector3.y();
    float z = rot.t_vector3.z();
    if (x != 0.f)
      heart = std::make_shared<RotateX>(heart, x);
    if (y != 0.f)
      heart = std::make_shared<RotateY>(heart, y);
    if (z != 0.f)
      heart = std::make_shared<RotateZ>(heart, z);
  }
  heart = std::make_shared<Translate>(heart, center.t_vector3);
  return BasicType(heart);
}

BasicType TranslateNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType object = object_expr->evaluate(env);
  BasicType offset = offset_expr->evaluate(env);
  return BasicType(
      std::make_shared<Translate>(object.t_object, offset.t_vector3));
}

BasicType ConstantMediumNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType object = object_expr->evaluate(env);
  BasicType density = density_expr->evaluate(env);
  BasicType albedo = albedo_expr->evaluate(env);
  return BasicType(std::make_shared<ConstantMedium>(
      object.t_object, density.t_float, albedo.t_vector3));
}

BasicType CameraNode::evaluate(std::shared_ptr<Environment> env) {
  BasicType shape, spp, depth, bg, from, at, vup, fov;
  if (shape_expr)
    shape = shape_expr->evaluate(env);
  else
    shape.t_pair =
        std::pair<std::shared_ptr<BasicType>, std::shared_ptr<BasicType>>(
            std::make_shared<BasicType>(600), std::make_shared<BasicType>(600));
  if (spp_expr)
    spp = spp_expr->evaluate(env);
  else
    spp.t_integer = 128;
  if (depth_expr)
    depth = depth_expr->evaluate(env);
  else
    depth.t_integer = 8;
  if (bg_expr)
    bg = bg_expr->evaluate(env);
  else
    bg.t_vector3 = Vec3(0, 0, 0);
  if (from_expr)
    from = from_expr->evaluate(env);
  else
    from.t_vector3 = Vec3(1000, 0, 0);
  if (at_expr)
    at = at_expr->evaluate(env);
  else
    at.t_vector3 = Vec3(0, 0, 0);
  if (vup_expr)
    vup = vup_expr->evaluate(env);
  else
    vup.t_vector3 = Vec3(0, 0, 1);
  if (fov_expr)
    fov = fov_expr->evaluate(env);
  else
    fov.t_float = 40.f;

  return BasicType(std::make_unique<Camera>(
      shape.t_pair.first->t_integer, shape.t_pair.second->t_integer,
      spp.t_integer, depth.t_integer, bg.t_vector3, from.t_vector3,
      at.t_vector3, vup.t_vector3, fov.t_float));
}

} // namespace parser

} // namespace tracer