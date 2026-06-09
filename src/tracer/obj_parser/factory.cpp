#include "tracer/obj_parser/factory.h"

namespace tracer {
namespace obj_parser {

Object::Object(const std::string &path) {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  source = buffer.str();
  if (source.empty()) {
    std::cerr << "加载模型文件失败: " << path << std::endl;
    return;
  }

  Lexer lexer(source);
  o_parser.init_settings(lexer.tokenizer());
  o_parser.parse();
  for (const auto &mtl : o_parser.mtl_labels) {
    Value v;
    mtl->evaluate(v);

    std::cout << "正在解析材质: " << v.v_mat_path << std::endl;
    std::ifstream mtl_file("../models/" + v.v_mat_path);
    std::stringstream mtl_buffer;
    mtl_buffer << mtl_file.rdbuf();
    std::string mtl_source = mtl_buffer.str();
    Lexer mtl_lexer(mtl_source);
    mtl_parser.init_settings(mtl_lexer.tokenizer());
    mtl_parser.parse_mtl();
    std::cout << "材质解析完成!" << std::endl;
  }
  mesh = std::make_unique<geometry::Mesh>();
  std::cout << "正在加载模型: " << path << std::endl;
  this->builder();
  std::cout << "模型加载完成!" << std::endl;
}

void Object::builder() {
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> texture_coords;
  std::vector<Face> faces;
  std::string mat_name;

  std::unordered_map<std::string, uint32_t> mat_map;

  Value value;
  uint32_t m_idx = 0;

  for (const auto &mesh_node : o_parser.meshes) {

    Value mtl_value;
    for (const auto &mtl_node : mtl_parser.nodes) {
      mtl_node->evaluate(mtl_value);
      mat_map[mtl_value.v_mat_name] = m_idx++;
    }

    for (const auto &mtl : mtl_value.v_params) {
      // Ns（光泽度） 转粗糙度公式
      float roughness = std::sqrt(2.0f / (mtl.Ns + 2.0f));

      // Ks（Specular） 转镜面/金属质感公式
      float spec_brightness =
          0.2126f * mtl.Ks[0] + 0.7152f * mtl.Ks[1] + 0.0722f * mtl.Ks[2];
      float metallic = std::clamp(spec_brightness, 0.0f, 1.0f);

      std::shared_ptr<material::StandardMaterial> mat;
      if (mtl.map_Kd.empty())
        mat = std::make_shared<material::StandardMaterial>(mtl.Kd, roughness,
                                                           metallic);
      else {
        mat = std::make_shared<material::StandardMaterial>(
            std::make_shared<texture::ImageTexture>(mtl.map_Kd.c_str()),
            roughness, metallic);
      }
      // Vec3 ambient_color = mtl.Ka; // 环境光遮蔽
      // Vec3 base_emissive = mtl.Ke;
      // mat->emissive = mtl.Ke + mtl.Ka * 0.1f;
      // mat.ior = mtl.ior;
      mesh->materials.push_back(mat);
    }

    if (mesh->materials.empty()) {
      mesh->materials.push_back(std::make_shared<material::StandardMaterial>(
          Vec3(0.8f, 0.8f, 0.8f), 1.0f, 0.0f));
    }
    for (const auto &node : mesh_node->nodes) {
      node->evaluate(value);
      if (value.v_type == Value::ValueType::V_VERTEX) {
        vertices.push_back(value.v_vertex);
      } else if (value.v_type == Value::ValueType::V_NORMAL) {
        normals.push_back(value.v_normal);
      } else if (value.v_type == Value::ValueType::V_TEXTURE) {
        texture_coords.push_back(value.v_texture_coord);
      } else if (value.v_type == Value::ValueType::V_FACE) {
        value.v_face.mat = mat_name;
        faces.push_back(value.v_face);
      } else if (value.v_type == Value::ValueType::V_MAT_NAME) {
        mat_name = value.v_mat_name;
      }
    }

    std::unordered_map<Index, uint32_t> vertex_map;
    for (const auto &face : faces) {
      if (!face.mat.empty()) {
        mesh->material_indices.push_back(mat_map[face.mat]);
      } else {
        mesh->material_indices.push_back(0);
      }

      for (size_t j = 0; j < 3; j++) {
        // 0: vertex index, 1: texture coord index, 2: normal index
        uint32_t vertex_index = face.v[0].idx[j] - 1;
        uint32_t texture_index = face.v[1].idx[j] - 1;
        uint32_t normal_index = face.v[2].idx[j] - 1;
        Index index{face.v[0].idx[j], face.v[1].idx[j], face.v[2].idx[j]};
        auto it = vertex_map.find(index);
        if (it == vertex_map.end()) {
          uint32_t idx = (uint32_t)mesh->vertices.size();
          mesh->vertices.push_back(geometry::Vertex{
              vertices[vertex_index],
              normal_index < normals.size() ? normals[normal_index]
                                            : Vec3(0.0f, 0.0f, 1.0f),
              texture_index < texture_coords.size()
                  ? texture_coords[texture_index]
                  : Vec2(),
          });
          vertex_map[index] = idx;
          mesh->indices.push_back(idx);
        } else {
          mesh->indices.push_back(it->second);
        }
      }
    }
  }
}

std::shared_ptr<geometry::Mesh> Object::take() { return std::move(mesh); }

} // namespace obj_parser
} // namespace tracer