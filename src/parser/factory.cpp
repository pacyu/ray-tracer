#include "tracer/parser/factory.h"

namespace tracer {

namespace parser {

Factory::Factory(const std::string &filename)
    : light(), world(), scenes(), types_map() {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  source = buffer.str();

  Pair image{600, 600};
  int spp = 128, max_depth = 8;
  Vec3 background(0, 0, 0), from(1000.0, 0.0, 0.0), at(0.0, 0.0, 0.0),
      vup(0.0, 0.0, 1.0);
  float fov = 40.0f;
  float aspect_ratio =
      static_cast<float>(image.x) / static_cast<float>(image.y);
  camera = std::make_unique<Camera>(
      static_cast<size_t>(image.x), static_cast<size_t>(image.y), spp,
      max_depth, background, from, at, vup, fov, aspect_ratio);
}

void Factory::parse() {
  Lexer lexer(source);
  auto tokens = lexer.tokenizer();

  Parser parser(tokens);
  parser.parse();

  SceneAST ast = parser.get_ast();

  for (const auto &object : ast.instances) {
    Instance instance;

    if (object._typename == "Int") {
      instance.kind = Instance::INT;
    } else if (object._typename == "Float") {
      instance.kind = Instance::FLOAT;
    } else if (object._typename == "Str" || object._typename == "String") {
      instance.kind = Instance::STRING;
    } else if (object._typename == "Pair") {
      instance.kind = Instance::PAIR;
    } else if (object._typename == "Vec3" || object._typename == "Vector3") {
      instance.kind = Instance::VECTOR3;
    } else if (object._typename == "Quat" || object._typename == "Quaternion") {
      instance.kind = Instance::QUATERNION;
    } else if (object._typename == "Camera") {
      instance.kind = Instance::CAMERA;
    } else if (object._typename == "Lam" || object._typename == "Lambertian") {
      instance.kind = Instance::LAMBERTIAN;
    } else if (object._typename == "M" || object._typename == "Metal") {
      instance.kind = Instance::METAL;
    } else if (object._typename == "G" || object._typename == "Glass" ||
               object._typename == "Dielectric") {
      instance.kind = Instance::DIELECTRIC;
    } else if (object._typename == "XYRect") {
      instance.kind = Instance::RECT;
    } else if (object._typename == "XZRect") {
      instance.kind = Instance::RECT;
    } else if (object._typename == "YZRect") {
      instance.kind = Instance::RECT;
    } else if (object._typename == "Box") {
      instance.kind = Instance::BOX;
    } else if (object._typename == "Sphere") {
      instance.kind = Instance::SPHERE;
    } else if (object._typename == "Heart") {
      instance.kind = Instance::HEART;
    } else if (object._typename == "CM" ||
               object._typename == "ConstantMedium") {
      instance.kind = Instance::CONSTANT_MEDIUM;
    }

    instance.name = object.varname;
    instance._typename = object._typename;
    for (const auto &argument : object.args) {
      instance.args.push_back(argument);
    }
    instance.arg = object.arg;
    scenes.push_back(instance);
  }
}

int Factory::build_integer(const Argument &argument) {
  int variant = std::get<int>(argument.value);
  return variant;
}

float Factory::build_float(const Argument &argument) {
  float variant = std::get<float>(argument.value);
  return variant;
}

Pair Factory::build_pair(const Argument &argument) {
  Pair variant = std::get<Pair>(argument.value);
  return variant;
}

Vec3 Factory::build_vector3(const Argument &argument) {
  Vec3 variant = std::get<Vec3>(argument.value);
  return variant;
}

Quaternion Factory::build_quaternion(const Argument &argument) {
  Quaternion variant = std::get<Quaternion>(argument.value);
  return variant;
}

std::string Factory::build_string(const Argument &argument) {
  std::string variant = std::get<std::string>(argument.value);
  return variant;
}

std::unique_ptr<Camera>
Factory::build_camera(const std::vector<Argument> &args) {
  Pair image{600, 600};
  int spp = 128, max_depth = 8;
  Vec3 background(0, 0, 0), from(1000.0, 0.0, 0.0), at(0.0, 0.0, 0.0),
      vup(0.0, 0.0, 1.0);
  float fov = 40.0f;

  for (const auto &argument : args) {
    switch (argument.type) {
    case Argument::INT:
      if (argument.name == "samples_per_pixel" || argument.name == "spp") {
        spp = std::get<int>(argument.value);
      } else if (argument.name == "max_depth") {
        max_depth = std::get<int>(argument.value);
      }
      break;
    case Argument::FLOAT:
      if (argument.name == "fov") {
        fov = std::get<float>(argument.value);
      }
      break;
    case Argument::PAIR:
      image = std::get<Pair>(argument.value);
      break;
    case Argument::VECTOR3:
      if (argument.name == "lookfrom" || argument.name == "from") {
        from = std::get<Vec3>(argument.value);
      } else if (argument.name == "lookat" || argument.name == "at") {
        at = std::get<Vec3>(argument.value);
      } else if (argument.name == "background" || argument.name == "bg") {
        background = std::get<Vec3>(argument.value);
      } else if (argument.name == "vup") {
        vup = std::get<Vec3>(argument.value);
      }
      break;
    default:
      break;
    }
  }
  float aspect_ratio =
      static_cast<float>(image.x) / static_cast<float>(image.y);
  return std::make_unique<Camera>(static_cast<size_t>(image.x),
                                  static_cast<size_t>(image.y), spp, max_depth,
                                  background, from, at, vup, fov, aspect_ratio);
}

std::shared_ptr<Material>
Factory::build_lambertian(const std::vector<Argument> &args) {
  Color albedo(0.0, 0.0, 0.0);
  std::string ref_name;

  for (const auto &argument : args) {
    switch (argument.type) {
    case Argument::VECTOR3:
      albedo = std::get<Vec3>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      albedo = types_map[ref_name].t_vector3;
      break;
    default:
      break;
    }
  }

  return std::make_shared<Lambertian>(albedo);
}

std::shared_ptr<Material>
Factory::build_metal(const std::vector<Argument> &args) {
  Color albedo(0.0, 0.0, 0.0);
  float fuzz = 0.5f;
  std::string ref_name;

  for (const auto &argument : args) {
    switch (argument.type) {
    case Argument::VECTOR3:
      albedo = std::get<Vec3>(argument.value);
      break;
    case Argument::FLOAT:
      fuzz = std::get<float>(argument.value);
      break;
    case Argument::STRING:
      if (argument.name == "albedo" || argument.name == "color") {
        ref_name = std::get<std::string>(argument.value);
        albedo = types_map[ref_name].t_vector3;
      } else if (argument.name == "fuzz") {
        ref_name = std::get<float>(argument.value);
        fuzz = types_map[ref_name].t_float;
      }
      break;
    default:
      break;
    }
  }

  return std::make_shared<Metal>(albedo, fuzz);
}

std::shared_ptr<Material>
Factory::build_dielectric(const std::vector<Argument> &args) {
  Color albedo(1.0, 1.0, 1.0);
  float density = 0.5f, ri = 1.0f;
  std::string ref_name;

  for (const auto &argument : args) {
    switch (argument.type) {
    case Argument::FLOAT:
      if (argument.name == "ri")
        ri = std::get<float>(argument.value);
      else if (argument.name == "density")
        density = std::get<float>(argument.value);
      break;
    case Argument::VECTOR3:
      albedo = std::get<Vec3>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "ri")
        ri = types_map[ref_name].t_float;
      else if (argument.name == "density")
        density = types_map[ref_name].t_float;
      else if (argument.name == "albedo" || argument.name == "color")
        albedo = types_map[ref_name].t_vector3;
      break;
    default:
      break;
    }
  }

  return std::make_shared<Dielectric>(albedo, density, ri);
}

std::shared_ptr<hittable> Factory::build_rect(const Instance &instance) {
  Quaternion quad{0, 0, 0, 0};
  float k = 1.0f;
  std::string material, ref_name;

  for (const auto &argument : instance.args) {
    switch (argument.type) {
    case Argument::QUATERNION:
      quad = std::get<Quaternion>(argument.value);
      break;
    case Argument::FLOAT:
      k = std::get<float>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "quat" || argument.name == "quaternion")
        quad = types_map[ref_name].t_quat;
      else if (argument.name == "k")
        k = types_map[ref_name].t_float;
      else if (argument.name == "mat" || argument.name == "material")
        material = ref_name;
      break;
    default:
      break;
    }
  }

  std::shared_ptr<hittable> table;

  if (instance._typename == "XYRect") {
    if (material == "Light") {
      light = std::make_shared<flip_face>(std::make_shared<XYRect>(
          quad.x, quad.y, quad.z, quad.w, k,
          std::make_shared<DiffuseLight>(Color(5, 5, 5))));
      table = light;
    } else {
      table = std::make_shared<XYRect>(quad.x, quad.y, quad.z, quad.w, k,
                                       types_map[material].t_material);
    }
  } else if (instance._typename == "XZRect") {
    if (material == "Light") {
      light = std::make_shared<flip_face>(std::make_shared<XZRect>(
          quad.x, quad.y, quad.z, quad.w, k,
          std::make_shared<DiffuseLight>(Color(5, 5, 5))));
      table = light;
    } else {
      table = std::make_shared<XZRect>(quad.x, quad.y, quad.z, quad.w, k,
                                       types_map[material].t_material);
    }
  } else if (instance._typename == "YZRect") {
    if (material == "Light") {
      light = std::make_shared<flip_face>(std::make_shared<YZRect>(
          quad.x, quad.y, quad.z, quad.w, k,
          std::make_shared<DiffuseLight>(Color(5, 5, 5))));
      table = light;
    } else {
      table = std::make_shared<YZRect>(quad.x, quad.y, quad.z, quad.w, k,
                                       types_map[material].t_material);
    }
  }
  return table;
}

std::shared_ptr<hittable> Factory::build_box(const Instance &instance) {
  Vec3 p0(0.0, 0.0, 0.0), p1(0.0, 0.0, 0.0), rotate(0.0, 0.0, 0.0);
  std::string material, ref_name;

  for (const auto &argument : instance.args) {
    switch (argument.type) {
    case Argument::VECTOR3:
      if (argument.name == "p0")
        p0 = std::get<Vec3>(argument.value);
      else if (argument.name == "p1")
        p1 = std::get<Vec3>(argument.value);
      else if (argument.name == "rot" || argument.name == "rotate")
        rotate = std::get<Vec3>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "p0")
        p0 = types_map[ref_name].t_vector3;
      else if (argument.name == "p1")
        p1 = types_map[ref_name].t_vector3;
      else if (argument.name == "rot" || argument.name == "rotate")
        rotate = types_map[ref_name].t_vector3;
      else if (argument.name == "mat" || argument.name == "material")
        material = ref_name;
      break;
    default:
      break;
    }
  }

  std::shared_ptr<hittable> box =
      std::make_shared<Box>(p0, p1, types_map[material].t_material);

  if (rotate.x() != 0)
    box = std::make_shared<RotateX>(box, rotate.x());
  if (rotate.y() != 0)
    box = std::make_shared<RotateY>(box, rotate.y());
  if (rotate.z() != 0)
    box = std::make_shared<RotateZ>(box, rotate.z());

  return box;
}

std::shared_ptr<hittable> Factory::build_sphere(const Instance &instance) {
  Vec3 center(0.0, 0.0, 0.0);
  float radius = 1.0f;
  std::string material, ref_name;

  for (const auto &argument : instance.args) {
    switch (argument.type) {
    case Argument::VECTOR3:
      center = std::get<Vec3>(argument.value);
      break;
    case Argument::FLOAT:
      radius = std::get<float>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "center")
        center = types_map[ref_name].t_vector3;
      else if (argument.name == "r" || argument.name == "radius")
        radius = types_map[ref_name].t_float;
      else if (argument.name == "mat" || argument.name == "material")
        material = ref_name;
      break;
    default:
      break;
    }
  }

  std::shared_ptr<hittable> sphere =
      std::make_shared<Sphere>(center, radius, types_map[material].t_material);

  return sphere;
}

std::shared_ptr<hittable> Factory::build_heart(const Instance &instance) {
  Vec3 center(0.0, 0.0, 0.0), rotate(0.0, 0.0, 0.0);
  float rho = 1.0f;
  std::string material, ref_name;

  for (const auto &argument : instance.args) {
    switch (argument.type) {
    case Argument::VECTOR3:
      if (argument.name == "rot" || argument.name == "rotate")
        rotate = std::get<Vec3>(argument.value);
      else
        center = std::get<Vec3>(argument.value);
      break;
    case Argument::FLOAT:
      rho = std::get<float>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "center")
        center = types_map[ref_name].t_vector3;
      else if (argument.name == "r" || argument.name == "rho")
        rho = types_map[ref_name].t_float;
      else if (argument.name == "rot" || argument.name == "rotate")
        rotate = types_map[ref_name].t_vector3;
      else if (argument.name == "mat" || argument.name == "material")
        material = ref_name;
      break;
    default:
      break;
    }
  }

  std::shared_ptr<hittable> heart = std::make_shared<Heart>(
      Vec3(0, 0, 0), rho, types_map[material].t_material);
  if (rotate.x() != 0)
    heart = std::make_shared<RotateX>(heart, rotate.x());
  if (rotate.y() != 0)
    heart = std::make_shared<RotateY>(heart, rotate.y());
  if (rotate.z() != 0)
    heart = std::make_shared<RotateZ>(heart, rotate.z());

  heart = std::make_shared<Translate>(heart, center);

  return heart;
}

std::shared_ptr<hittable>
Factory::build_constant_medium(const Instance &instance) {
  Vec3 albedo(0.0, 0.0, 0.0);
  float density = 0.0f;
  std::string object_name, ref_name;

  for (const auto &argument : instance.args) {
    switch (argument.type) {
    case Argument::FLOAT:
      density = std::get<float>(argument.value);
      break;
    case Argument::VECTOR3:
      albedo = std::get<Vec3>(argument.value);
      break;
    case Argument::STRING:
      ref_name = std::get<std::string>(argument.value);
      if (argument.name == "albedo" || argument.name == "color")
        albedo = types_map[ref_name].t_vector3;
      else if (argument.name == "density")
        density = types_map[ref_name].t_float;
      else if (argument.name == "shape" || argument.name == "object")
        object_name = ref_name;
      break;
    default:
      break;
    }
  }

  std::shared_ptr<hittable> boundry = std::make_shared<ConstantMedium>(
      types_map[object_name].t_object, density, albedo);

  return boundry;
}

void Factory::builder() {
  for (const auto &scene : scenes) {
    switch (scene.kind) {
    case Instance::INT:
      types_map[scene.name].t_integer = build_integer(scene.arg);
      break;
    case Instance::FLOAT:
      types_map[scene.name].t_float = build_float(scene.arg);
      break;
    case Instance::PAIR:
      types_map[scene.name].t_pair = build_pair(scene.arg);
      break;
    case Instance::VECTOR3:
      types_map[scene.name].t_vector3 = build_vector3(scene.arg);
      break;
    case Instance::QUATERNION:
      types_map[scene.name].t_quat = build_quaternion(scene.arg);
      break;
    case Instance::STRING:
      types_map[scene.name].t_string = build_string(scene.arg);
      break;
    default:
      break;
    }
  }

  for (const auto &scene : scenes) {
    switch (scene.kind) {
    case Instance::LAMBERTIAN:
      types_map[scene.name].t_material = build_lambertian(scene.args);
      break;
    case Instance::METAL:
      types_map[scene.name].t_material = build_metal(scene.args);
      break;
    case Instance::DIELECTRIC:
      types_map[scene.name].t_material = build_dielectric(scene.args);
      break;
    default:
      break;
    }
  }

  for (const auto &scene : scenes) {
    std::shared_ptr<hittable> obj = nullptr;
    switch (scene.kind) {
    case Instance::CAMERA:
      camera = build_camera(scene.args);
      break;
    case Instance::RECT:
      obj = build_rect(scene);
      break;
    case Instance::BOX:
      obj = build_box(scene);
      break;
    case Instance::SPHERE:
      obj = build_sphere(scene);
      break;
    case Instance::HEART:
      obj = build_heart(scene);
      break;
    default:
      break;
    }
    if (obj) {
      types_map[scene.name].t_object = obj;
      world.add(obj);
    }
  }

  for (const auto &scene : scenes) {
    std::shared_ptr<hittable> obj = nullptr;
    switch (scene.kind) {
    case Instance::CONSTANT_MEDIUM:
      obj = build_constant_medium(scene);
      break;
    default:
      break;
    }
    if (obj) {
      types_map[scene.name].t_object = obj;
      world.add(obj);
    }
  }
}

} // namespace parser

} // namespace tracer