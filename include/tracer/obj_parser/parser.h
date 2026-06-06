#pragma once
#include "tracer/geometry/triangle.h"
#include "tracer/obj_parser/ast.h"
#include "tracer/obj_parser/token.h"

namespace tracer {
namespace obj_parser {

class ObjParser {
public:
  std::vector<std::unique_ptr<MeshNode>> meshes;
  std::vector<std::shared_ptr<ASTNode>> nodes;
  std::vector<std::unique_ptr<MtlLibNode>> mtl_labels;
  ObjParser() {}
  ObjParser(const std::vector<Token> &);
  void init_settings(const std::vector<Token> &);
  void parse();
  void parse_mtl();

private:
  std::vector<Token> tokens;
  size_t cursor = 0;

  void exception_message(TokenType, TokenType);
  Token opt();
  Token peek();
  Token expect(TokenType);
  void consume();

  float make_number();
  Vec3 make_vector3();

  std::shared_ptr<SmoothNode> make_smooth();
  std::shared_ptr<VertexNode> make_vertex();
  std::shared_ptr<NormalNode> make_normal();
  std::shared_ptr<TextureCoordNode> make_texture_coord();
  void make_face();
  MtlLibNode make_mtllib();
  std::shared_ptr<UseMtlNode> make_usemtl();
  void make_mesh();

  std::shared_ptr<TexParamsNode> make_newmtl();
};

} // namespace obj_parser
} // namespace tracer