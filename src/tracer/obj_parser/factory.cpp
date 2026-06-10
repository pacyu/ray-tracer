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
  size_t last_sep = path.find_last_of("/\\");
  if (last_sep != std::string::npos) {
    dir = path.substr(0, last_sep); // 不包含分隔符
  }
  Lexer lexer(source);
  o_parser.init_settings(lexer.tokenizer());
  o_parser.parse();
  for (const auto &mtl : o_parser.mtl_labels) {
    Value v;
    mtl->evaluate(v);

    std::cout << "正在解析材质: " << v.v_mat_path << std::endl;
    std::ifstream mtl_file(dir + "/" + v.v_mat_path);
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

  std::unordered_map<std::string, uint32_t> mat_map;

  Value value;
  Value mtl_value;

  for (const auto &node : o_parser.nodes) {
    node->evaluate(value);
    if (value.v_type == Value::ValueType::V_VERTEX) {
      vertices.push_back(value.v_vertex);
    } else if (value.v_type == Value::ValueType::V_NORMAL) {
      normals.push_back(value.v_normal);
    } else if (value.v_type == Value::ValueType::V_TEXTURE) {
      texture_coords.push_back(value.v_texture_coord);
    } else if (value.v_type == Value::ValueType::V_FACE) {
      // 正常来说，usemtl 在 f 前出现，所以，按出现顺序，f 块设置最新的材质名
      value.v_face.mat = value.v_usemtl_name;
      faces.push_back(value.v_face);
    }
    // 其他属性暂时忽略
  }

  for (const auto &mtl_node : mtl_parser.nodes) {
    mtl_node->evaluate(mtl_value);
  }

  uint32_t m_idx = 0;
  for (const auto &param : mtl_value.v_params) {
    mat_map[param.mat_name] = m_idx++;

    // 粗糙度：Ns (Phong 高光指数) → roughness
    float roughness = std::sqrt(2.0f / (param.Ns + 2.0f));
    // 金属度：从 Ks 亮度估算
    float spec_brightness =
        0.2126f * param.Ks[0] + 0.7152f * param.Ks[1] + 0.0722f * param.Ks[2];
    float metallic = std::clamp(spec_brightness, 0.0f, 1.0f);
    // 不透明度 alpha：优先使用 d，否则用 1-Tr
    float alpha = 1.0f;
    if (param.d != 1.0f)
      alpha = param.d;
    else if (param.Tr != 0.0f)
      alpha = 1.0f - param.Tr;
    alpha = std::clamp(alpha, 0.0f, 1.0f);

    // 创建材质实例
    auto mat = std::make_shared<material::StandardMaterial>();

    // 基础色 albedo (map_Kd 或 Kd)
    if (param.map_Kd.empty()) {
      mat->albedo = std::make_shared<texture::SolidColor>(param.Kd);
    } else {
      mat->albedo = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Kd).c_str());
    }

    // 粗糙度贴图：优先使用 PBR map_Pr，否则使用 map_Ns
    if (!param.map_Pr.empty()) {
      mat->roughness_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Pr).c_str());
    } else if (!param.map_Ns.empty()) {
      mat->roughness_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Ns).c_str());
    } else {
      mat->roughness = roughness;
    }

    // 金属度贴图：优先使用 PBR map_Pm，否则使用 map_Ks
    if (!param.map_Pm.empty()) {
      mat->metallic_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Pm).c_str());
    } else if (!param.map_Ks.empty()) {
      mat->metallic_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Ks).c_str());
    } else {
      mat->metallic = metallic;
    }

    // AO 贴图：优先使用 PBR map_Po，否则使用 map_Ka
    if (!param.map_Po.empty()) {
      mat->ambient_occlusion = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Po).c_str());
    } else if (!param.map_Ka.empty()) {
      mat->ambient_occlusion = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Ka).c_str());
    }

    // 自发光 emissive
  
    if (!param.map_Ke.empty()) {
      mat->emissive_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Ke).c_str());
    } else if (!param.map_Pe.empty()) {
      mat->emissive_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Pe).c_str());
    } else {
      mat->emissive_map = std::make_shared<texture::SolidColor>(param.Ke);
    }

    // 法线贴图 (map_bump / bump)
    if (!param.map_bump.empty()) {
      mat->normal_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_bump).c_str());
    } else if (!param.bump.empty()) {
      mat->normal_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.bump).c_str());
    }

    // 透明度贴图 (map_d / map_Tr)
    if (!param.map_d.empty()) {
      mat->alpha_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_d).c_str());
    } else if (!param.map_Tr.empty()) {
      mat->alpha_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_Tr).c_str());
    } else {
      mat->alpha = alpha;
    }

    // 反射贴图 (map_refl / refl)
    if (!param.map_refl.empty()) {
      mat->reflection_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.map_refl).c_str());
    } else if (!param.refl.empty()) {
      mat->reflection_map = std::make_shared<texture::ImageTexture>(
          (dir + "/" + param.refl).c_str());
    }

    // 透射滤镜颜色 Tf
    mat->tf_color = param.Tf;

    // 反射清晰度 Sharpness
    mat->sharpness = param.sharpness;

    // 折射率与光照模型
    mat->ior = param.Ni;
    mat->illum_model = param.illum;

    // 双面渲染：根据 illum 模型判断
    if (param.illum == 4 || param.illum == 6 || param.illum == 9) {
      mat->double_sided = true;
    }

    // 镜面反射基准 specular (F0)
    float spec_val = std::max({param.Ks[0], param.Ks[1], param.Ks[2]});
    mat->specular = std::clamp(spec_val, 0.0f, 1.0f);

    mesh->materials.push_back(mat);
  }

  // 保底，假如没有 mtl 文件的话，使用默认金属材质
  if (mesh->materials.empty()) {
    mesh->materials.push_back(std::make_shared<material::StandardMaterial>(
        Vec3(0.8f, 0.8f, 0.8f), 1.0f, 0.0f));
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

std::shared_ptr<geometry::Mesh> Object::take() { return std::move(mesh); }

} // namespace obj_parser
} // namespace tracer