#pragma once
#include "tracer/geometry/mesh.h"
#include "tracer/math/vec2.h"
#include "tracer/math/vec3.h"
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace tracer {
namespace obj_parser {

struct Index {
  uint32_t idx[3] = {0};
  bool operator==(const Index &other) const {
    return idx[0] == other.idx[0] && idx[1] == other.idx[1] &&
           idx[2] == other.idx[2];
  }
};

struct Face {
  std::string mat;
  Index v[3]; // vertex index, texture coord index, normal index
};

struct MaterialParams {
  std::string mat_name;
  Vec3 Ka;                // Ambient 颜色 - 环境光反射系数
  Vec3 Kd;                // Diffuse 颜色 - 漫反射颜色
  Vec3 Ks;                // Specular 颜色 - 高光颜色
  Vec3 Ke;                // Emissive 颜色 - 自发光颜色
  float Ns = 32.0f;       // Shininess - 高光指数 / 光泽度
  float Ni = 1.5f;        // 折射率
  float d = 1.0f;         // Opacity = 不透明度
  float Tr = 0.0f;        // Transparency - 透明度
  Vec3 Tf;                // Transmission filter - 透射滤镜颜色
  int illum = 2;          // 光照模型
  float sharpness = 0.0f; // 反射贴图清晰度

  // 标准纹理贴图
  std::string map_Kd;   // 漫反射纹理
  std::string map_Ka;   // 环境光纹理
  std::string map_Ks;   // 高光纹理（可转金属度）
  std::string map_Ke;   // 自发光纹理
  std::string map_Ns;   // 光泽度纹理（转粗糙度）
  std::string map_d;    // 不透明度纹理
  std::string map_Tr;   // 透明度纹理（与 map_d 类似）
  std::string map_bump; // 凹凸纹理（法线贴图或灰度凹凸）
  std::string bump;     // 凹凸纹理（旧式，同 map_bump）
  std::string map_refl; // 反射纹理
  std::string refl;     // 反射贴图（旧式，同 map_refl）

  // PBR 扩展纹理（金属度/粗糙度/AO 工作流）
  std::string map_Pr; // 粗糙度贴图（Roughness）
  std::string map_Pm; // 金属度贴图（Metallic）
  std::string map_Po; // 环境光遮蔽贴图（AO）
  std::string map_Ps; // 镜面反射贴图（Specular）
  std::string map_Pe; // 自发光贴图（Emission，同 map_Ke）

  // 高级贴图
  std::string disp;  // 位移贴图（Displacement）
  std::string decal; // 贴花贴图（Decal）
};

struct Value {
  int v_number;
  Vec3 v_vertex;
  Vec3 v_normal;
  Vec2 v_texture_coord;
  Face v_face;
  std::vector<MaterialParams> v_params;
  std::string v_usemtl_name;
  std::string v_mat_path;

  enum class ValueType {
    V_NUMBER,
    V_VERTEX,
    V_NORMAL,
    V_TEXTURE,
    V_FACE,
    V_TEX_PARAMS,
    V_USEMTL_NAME,
    V_MAT_PATH,
    V_MAT_NAME
  } v_type;
};

struct ASTNode {
  virtual ~ASTNode() = default;
  virtual void evaluate(Value &) = 0;
};

class MtlLibNode : public ASTNode {
public:
  virtual void evaluate(Value &) override;
  MtlLibNode() {}
  MtlLibNode(const std::string &);

private:
  std::string value;
};

class VertexNode : public ASTNode {
public:
  void evaluate(Value &) override;
  VertexNode() {};
  VertexNode(const Vec3 &);

private:
  Vec3 value;
};

class NormalNode : public ASTNode {
public:
  void evaluate(Value &) override;
  NormalNode() {}
  NormalNode(const Vec3 &);

private:
  Vec3 value;
};

class TextureCoordNode : public ASTNode {
public:
  void evaluate(Value &) override;
  TextureCoordNode() {}
  TextureCoordNode(const Vec2 &);

private:
  Vec2 value;
};

class ObjectNode : public ASTNode {
public:
  void evaluate(Value &) override;
  ObjectNode(const std::string &);

private:
  std::string value;
};

class GroupNode : public ASTNode {
public:
  void evaluate(Value &) override;
  GroupNode(const std::string &);

private:
  std::string value;
};

class UseMtlNode : public ASTNode {
public:
  void evaluate(Value &) override;
  UseMtlNode() {}
  UseMtlNode(const std::string &);

private:
  std::string value;
};

class SmoothNode : public ASTNode {
public:
  void evaluate(Value &) override;
  SmoothNode(const std::string &);
  SmoothNode(int number);

private:
  int value = 0;
  std::string str_value;
};

class FaceNode : public ASTNode {
public:
  void evaluate(Value &) override;
  FaceNode() {}
  FaceNode(const Face &);

private:
  Face value;
};

class MaterialParamsNode : public ASTNode {
public:
  void evaluate(Value &) override;
  MaterialParamsNode() {}
  MaterialParamsNode(const MaterialParams &);

private:
  MaterialParams value;
};

} // namespace obj_parser
} // namespace tracer

namespace std {
template <> struct hash<tracer::obj_parser::Index> {
  size_t operator()(const tracer::obj_parser::Index &k) const {
    return ((hash<uint32_t>()(k.idx[0]) * 31 + hash<uint32_t>()(k.idx[1])) *
                31 +
            hash<uint32_t>()(k.idx[2]));
  }
};
} // namespace std