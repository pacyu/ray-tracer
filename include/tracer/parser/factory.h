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
  const std::shared_ptr<flip_face> &get_light() const { return light; }
  const hittable_list &get_world() const { return world; }

private:
  std::string source;

  std::unique_ptr<Camera> camera;
  std::shared_ptr<flip_face> light;
  hittable_list world;
  std::vector<Instance> scenes;

  std::unordered_map<std::string, BasicType> types_map;

  int build_integer(const Argument &arg);
  float build_float(const Argument &arg);
  Pair build_pair(const Argument &arg);
  Vec3 build_vector3(const Argument &arg);
  Quaternion build_quaternion(const Argument &arg);
  std::string build_string(const Argument &args);
  std::unique_ptr<Camera> build_camera(const std::vector<Argument> &args);
  std::shared_ptr<Material> build_lambertian(const std::vector<Argument> &args);
  std::shared_ptr<Material> build_metal(const std::vector<Argument> &args);
  std::shared_ptr<Material> build_dielectric(const std::vector<Argument> &args);
  std::shared_ptr<hittable> build_rect(const Instance &instance);
  std::shared_ptr<hittable> build_box(const Instance &instance);
  std::shared_ptr<hittable> build_sphere(const Instance &instance);
  std::shared_ptr<hittable> build_heart(const Instance &instance);
  std::shared_ptr<hittable> build_constant_medium(const Instance &instance);
};

} // namespace parser

} // namespace tracer