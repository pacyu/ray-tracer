#include "tracer/parser/factory.h"

namespace tracer {

namespace parser {

Factory::Factory(const std::string &filename) : camera(), light(), world() {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  source = buffer.str();
}

void Factory::builder() {
  Lexer lexer(source);
  auto tokens = lexer.tokenizer();

  Parser parser(tokens);
  parser.parse();

  SceneAST ast = parser.get_ast();

  std::unordered_map<std::string, std::shared_ptr<Material>> material_map;
  std::unordered_map<std::string, std::shared_ptr<hittable>> object_map;

  for (const auto &obj : ast.objects) {
    if (obj.type == "Camera") {
      size_t image_width;
      size_t image_height;
      float aspect_ratio;
      int samples_per_pixel;
      int max_depth;
      Color background;
      Point3 lookfrom, lookat;
      Vec3 vup;
      float fov;

      for (const auto &property : obj.properties) {
        if (property.name == "image_size") {
          pair size = std::get<pair>(property.value);
          image_width = size.x;
          image_height = size.y;
          aspect_ratio = static_cast<float>(image_width) /
                         static_cast<float>(image_height);
        } else if (property.name == "background") {
          background = std::get<Vec3>(property.value);
        } else if (property.name == "samples_per_pixel") {
          samples_per_pixel = std::get<int>(property.value);
        } else if (property.name == "max_depth") {
          max_depth = std::get<int>(property.value);
        } else if (property.name == "lookfrom")
          lookfrom = std::get<Vec3>(property.value);
        else if (property.name == "lookat")
          lookat = std::get<Vec3>(property.value);
        else if (property.name == "vup")
          vup = std::get<Vec3>(property.value);
        else if (property.name == "fov")
          fov = std::get<float>(property.value);
      }
      camera = Camera(image_width, image_height, samples_per_pixel, max_depth,
                      background, lookfrom, lookat, vup, fov, aspect_ratio);

    } else if (obj.type.rfind("Lambertian", 0) != std::string::npos) {
      for (const auto &property : obj.properties) {
        Vec3 color = std::get<Vec3>(property.value);
        auto material = std::make_shared<Lambertian>(color);
        material_map[obj.type] = material;
      }

    } else if (obj.type.rfind("Metal", 0) != std::string::npos) {
      Vec3 color;
      float fuzz;
      for (const auto &property : obj.properties) {
        if (property.name == "albedo")
          color = std::get<Vec3>(property.value);
        else if (property.name == "fuzz")
          fuzz = std::get<float>(property.value);
      }
      auto material = std::make_shared<Metal>(color, fuzz);
      material_map[obj.type] = material;

    } else if (obj.type.rfind("Dielectric", 0) != std::string::npos) {
      Vec3 color;
      float ri;
      for (const auto &property : obj.properties) {
        if (property.name == "albedo")
          color = std::get<Vec3>(property.value);
        else if (property.name == "ri")
          ri = std::get<float>(property.value);
      }
      auto material = std::make_shared<Dielectric>(color, ri);
      material_map[obj.type] = material;

    } else if (obj.type.rfind("DiffuseLight", 0) != std::string::npos) {
      for (const auto &property : obj.properties) {
        Vec3 color = std::get<Vec3>(property.value);
        auto material = std::make_shared<DiffuseLight>(color);
        material_map[obj.type] = material;
      }

    } else if (obj.type.find("Rect") != std::string::npos) {
      quaternion q;
      float k;
      std::string material_name;

      for (const auto &property : obj.properties) {
        if (property.name == "quaternion")
          q = std::get<quaternion>(property.value);
        else if (property.name == "k")
          k = std::get<float>(property.value);
        else if (property.name == "material") {
          material_name = std::get<std::string>(property.value);
        }
      }

      if (obj.type.rfind("XYRect", 0) != std::string::npos) {
        std::shared_ptr<XYRect> rect = std::make_shared<XYRect>(
            q.x, q.y, q.z, q.w, k, material_map[material_name]);
        world.add(rect);
        object_map[obj.type] = rect;
      } else if (obj.type.rfind("XZRect", 0) != std::string::npos) {
        std::shared_ptr<XZRect> rect = std::make_shared<XZRect>(
            q.x, q.y, q.z, q.w, k, material_map[material_name]);
        if (material_name.find("Light") == std::string::npos)
          world.add(rect);
        object_map[obj.type] = rect;
      } else if (obj.type.rfind("YZRect", 0) != std::string::npos) {
        std::shared_ptr<YZRect> rect = std::make_shared<YZRect>(
            q.x, q.y, q.z, q.w, k, material_map[material_name]);
        world.add(rect);
        object_map[obj.type] = rect;
      }

    } else if (obj.type.rfind("Light", 0) != std::string::npos) {
      for (const auto &property : obj.properties) {
        if (property.name == "object") {
          std::string object_name = std::get<std::string>(property.value);
          light = std::make_shared<flip_face>(object_map[object_name]);
          world.add(light);
        }
      }
    } else if (obj.type.rfind("Sphere", 0) != std::string::npos) {
      Point3 center;
      float radius;
      std::string material_name;

      for (const auto &property : obj.properties) {
        if (property.name == "center")
          center = std::get<Vec3>(property.value);
        else if (property.name == "radius")
          radius = std::get<float>(property.value);
        else if (property.name == "material") {
          material_name = std::get<std::string>(property.value);
        }
      }
      std::shared_ptr<Sphere> sphere =
          std::make_shared<Sphere>(center, radius, material_map[material_name]);
      world.add(sphere);
    } else if (obj.type.rfind("Box", 0) != std::string::npos) {
      Point3 p0, p1;
      std::string material_name;

      for (const auto &property : obj.properties) {
        if (property.name == "p0")
          p0 = std::get<Vec3>(property.value);
        else if (property.name == "p1")
          p1 = std::get<Vec3>(property.value);
        else if (property.name == "material") {
          material_name = std::get<std::string>(property.value);
        }
      }
      std::shared_ptr<Box> box =
          std::make_shared<Box>(p0, p1, material_map[material_name]);
      world.add(box);
    } else if (obj.type.rfind("Heart", 0) != std::string::npos) {
      Point3 center;
      float radius;
      std::string material_name;

      for (const auto &property : obj.properties) {
        if (property.name == "center")
          center = std::get<Vec3>(property.value);
        else if (property.name == "radius")
          radius = std::get<float>(property.value);
        else if (property.name == "material") {
          material_name = std::get<std::string>(property.value);
        }
      }
      std::shared_ptr<Heart> heart =
          std::make_shared<Heart>(center, radius, material_map[material_name]);
      world.add(heart);
    } else {
      std::cerr << "Unknown object type: " << obj.type << std::endl;
    }
  }
}

} // namespace parser

} // namespace tracer