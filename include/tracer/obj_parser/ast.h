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
           idx[1] == other.idx[1];
  }
};

struct Face {
  std::string mat;
  Index v[3]; // vertex index, texture coord index, normal index
};

struct TexParams {
  std::string mat_name;
  Vec3 Ka;            // Ambient 颜色 - 环境光反射系数
  Vec3 Kd;            // Diffuse 颜色 - 漫反射颜色
  Vec3 Ks;            // Specular 颜色 - 高光颜色
  float Ns;           // Shininess - 高光指数 / 光泽度
  float d;            // Opacity = 不透明度
  float Tr;           // Transparency - 透明度
  int illum;          // 光照模型
  std::string map_Kd; // 漫反射纹理
};

struct Value {
  int v_number;
  Vec3 v_vertex;
  Vec3 v_normal;
  Vec2 v_texture_coord;
  Face v_face;
  std::vector<TexParams> v_params;
  std::unique_ptr<geometry::Mesh> v_mesh;
  std::string v_mat_path;
  std::string v_mat_name;

  enum class ValueType {
    V_NUMBER,
    V_VERTEX,
    V_NORMAL,
    V_TEXTURE,
    V_FACE,
    V_TEX_PARAMS,
    V_MESH,
    V_MAT_PATH,
    V_MAT_NAME
  } v_type;
};

struct ASTNode {
  virtual ~ASTNode() = default;
  virtual void evaluate(Value &) = 0;
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

class FaceNode : public ASTNode {
public:
  void evaluate(Value &) override;
  FaceNode() {}
  FaceNode(const Face &);

private:
  Face value;
};

class MtlLibNode : public ASTNode {
public:
  virtual void evaluate(Value &) override;
  MtlLibNode() {}
  MtlLibNode(const std::string &);

private:
  std::string value;
};

class UseMtlNode : public ASTNode {
public:
  virtual void evaluate(Value &) override;
  UseMtlNode() {}
  UseMtlNode(const std::string &);

private:
  std::string value;
};

class TexParamsNode : public ASTNode {
public:
  void evaluate(Value &) override;
  TexParamsNode() {}
  TexParamsNode(const TexParams &);

private:
  TexParams value;
};

class MeshNode {
public:
  MeshNode() {}

  std::string mesh_name;
  std::vector<std::shared_ptr<ASTNode>> nodes;
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