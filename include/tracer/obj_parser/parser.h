#pragma once
#include "tracer/geometry/triangle.h"
#include "tracer/obj_parser/ast.h"
#include "tracer/obj_parser/token.h"

namespace tracer {
namespace obj_parser {

class ObjParser {
public:
  std::vector<std::unique_ptr<ASTNode>> nodes;
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
  std::string make_string();
  Vec3 make_vector3();

  std::unique_ptr<MtlLibNode> make_mtllib();
  std::unique_ptr<VertexNode> make_vertex();
  std::unique_ptr<NormalNode> make_normal();
  std::unique_ptr<TextureCoordNode> make_texture_coord();
  std::unique_ptr<ObjectNode> make_object();
  std::unique_ptr<GroupNode> make_group();
  std::unique_ptr<UseMtlNode> make_usemtl();
  std::unique_ptr<SmoothNode> make_smooth();
  void make_face();

  std::unique_ptr<MaterialParamsNode> make_newmtl();
};

} // namespace obj_parser
} // namespace tracer