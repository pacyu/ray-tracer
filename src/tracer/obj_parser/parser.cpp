#include "tracer/obj_parser/parser.h"

namespace tracer {
namespace obj_parser {

ObjParser::ObjParser(const std::vector<Token> &tokens) : tokens(tokens) {}

void ObjParser::init_settings(const std::vector<Token> &tokens) {
  this->tokens = tokens;
}

void ObjParser::exception_message(TokenType expect_type,
                                  TokenType actual_type) {
  throw "Expected a type '" + get_type(expect_type) + "', but got '" +
      get_type(actual_type) + "'.";
}

Token ObjParser::opt() {
  if (cursor < tokens.size())
    return tokens[cursor];
  else
    return Token(TokenType::ENDOFFILE, "");
}

Token ObjParser::peek() {
  if (cursor + 1 < tokens.size()) {
    return tokens[cursor + 1];
  }
  return Token(TokenType::ENDOFFILE, "");
}

void ObjParser::consume() { ++cursor; }

Token ObjParser::expect(TokenType t_type) {
  if (t_type == opt().type) {
    return opt();
  }
  exception_message(t_type, opt().type);
}

float ObjParser::make_number() {
  Token token = opt();
  consume();
  float number = std::stof(std::string(token.lexeme));
  return number;
}

Vec3 ObjParser::make_vector3() {
  expect(TokenType::NUMBER);
  float x = make_number();
  expect(TokenType::NUMBER);
  float y = make_number();
  expect(TokenType::NUMBER);
  float z = make_number();
  return Vec3(x, y, z);
}

std::shared_ptr<SmoothNode> ObjParser::make_smooth() {
  Token token = opt();
  if (token.type == TokenType::NUMBER) {
    int n = static_cast<int>(make_number());
    return std::make_shared<SmoothNode>(n);
  }
  consume();
  return std::make_shared<SmoothNode>(std::string(token.lexeme));
}

std::shared_ptr<VertexNode> ObjParser::make_vertex() {
  expect(TokenType::NUMBER);
  float x = make_number();
  expect(TokenType::NUMBER);
  float y = make_number();
  expect(TokenType::NUMBER);
  float z = make_number();
  return std::make_shared<VertexNode>(Vec3(x, y, z));
}

std::shared_ptr<NormalNode> ObjParser::make_normal() {
  expect(TokenType::NUMBER);
  float x = make_number();
  expect(TokenType::NUMBER);
  float y = make_number();
  expect(TokenType::NUMBER);
  float z = make_number();
  return std::make_shared<NormalNode>(Vec3(x, y, z));
}

std::shared_ptr<TextureCoordNode> ObjParser::make_texture_coord() {
  expect(TokenType::NUMBER);
  float x = make_number();
  expect(TokenType::NUMBER);
  float y = make_number();
  return std::make_shared<TextureCoordNode>(Vec2{x, y});
}

void ObjParser::make_face() {
  Face face1;

  expect(TokenType::NUMBER);
  uint32_t vertex1 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t coord1 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t normal1 = (uint32_t)make_number();

  expect(TokenType::NUMBER);
  uint32_t vertex2 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t coord2 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t normal2 = (uint32_t)make_number();

  expect(TokenType::NUMBER);
  uint32_t vertex3 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t coord3 = (uint32_t)make_number();
  expect(TokenType::SLASH);
  consume();
  expect(TokenType::NUMBER);
  uint32_t normal3 = (uint32_t)make_number();

  // 第一个面 (1, 2, 3)
  face1.v[0].idx[0] = vertex1;
  face1.v[0].idx[1] = vertex2;
  face1.v[0].idx[2] = vertex3;

  face1.v[1].idx[0] = coord1;
  face1.v[1].idx[1] = coord2;
  face1.v[1].idx[2] = coord3;

  face1.v[2].idx[0] = normal1;
  face1.v[2].idx[1] = normal2;
  face1.v[2].idx[2] = normal3;

  nodes.push_back(std::make_shared<FaceNode>(face1));

  // 如果接下来的 token 是数值类型，说明该面是一个四边形
  Token token = opt();
  if (token.type == TokenType::NUMBER) {
    // 第二面的顶点为 (1, 3, 4)
    Face face2;

    expect(TokenType::NUMBER);
    uint32_t vertex4 = (uint32_t)make_number();
    expect(TokenType::SLASH);
    consume();
    expect(TokenType::NUMBER);
    uint32_t coord4 = (uint32_t)make_number();
    expect(TokenType::SLASH);
    consume();
    expect(TokenType::NUMBER);
    uint32_t normal4 = (uint32_t)make_number();

    face2.v[0].idx[0] = vertex1;
    face2.v[0].idx[1] = vertex3;
    face2.v[0].idx[2] = vertex4;

    face2.v[1].idx[0] = coord1;
    face2.v[1].idx[1] = coord3;
    face2.v[1].idx[2] = coord4;

    face2.v[2].idx[0] = normal1;
    face2.v[2].idx[1] = normal3;
    face2.v[2].idx[2] = normal4;

    nodes.push_back(std::make_shared<FaceNode>(face2));
  }
}

MtlLibNode ObjParser::make_mtllib() {
  expect(TokenType::IDENTIFIER);
  Token token = opt();
  consume();
  return MtlLibNode(std::string(token.lexeme));
}

std::shared_ptr<UseMtlNode> ObjParser::make_usemtl() {
  expect(TokenType::IDENTIFIER);
  Token token = opt();
  consume();
  return std::make_shared<UseMtlNode>(std::string(token.lexeme));
}

std::shared_ptr<TexParamsNode> ObjParser::make_newmtl() {
  TexParams tex_params;
  while (opt().type != TokenType::NEWMTL &&
         opt().type != TokenType::ENDOFFILE) {
    Token token = opt();
    consume();
    if (token.type == TokenType::IDENTIFIER) {
      tex_params.mat_name = std::string(token.lexeme);
    } else if (token.type == TokenType::AMBIENT) {
      tex_params.Ka = make_vector3();
    } else if (token.type == TokenType::DIFFUSE) {
      tex_params.Kd = make_vector3();
    } else if (token.type == TokenType::SPECULAR) {
      tex_params.Ks = make_vector3();
    } else if (token.type == TokenType::SHININESS) {
      tex_params.Ns = make_number();
    } else if (token.type == TokenType::OPACITY) {
      tex_params.d = make_number();
    } else if (token.type == TokenType::MAP_KD) {
      token = opt();
      tex_params.map_Kd = std::string(token.lexeme);
      consume();
    } else {
      break;
    }
  }

  return std::make_shared<TexParamsNode>(tex_params);
}

void ObjParser::parse_mtl() {
  while (cursor < tokens.size()) {
    Token token = opt();
    std::shared_ptr<ASTNode> node;

    consume();

    if (token.type == TokenType::ENDOFFILE) {
      break;
    } else if (token.type == TokenType::NEWMTL) {
      node = make_newmtl();
    } else {
      continue;
    }
    nodes.push_back(node);
  }
}

void ObjParser::make_mesh() {
  while (opt().type != TokenType::OBJECT ||
         opt().type != TokenType::ENDOFFILE) {
    Token token = opt();
    std::shared_ptr<ASTNode> node;

    consume();

    if (token.type == TokenType::SMOOTH) {
      node = make_smooth();
    } else if (token.type == TokenType::VERTEX) {
      node = make_vertex();
    } else if (token.type == TokenType::TEXTURE_COORD) {
      node = make_texture_coord();
    } else if (token.type == TokenType::NORMAL) {
      node = make_normal();
    } else if (token.type == TokenType::FACE) {
      make_face();
      continue;
    } else if (token.type == TokenType::USEMTL) {
      node = make_usemtl();
    } else {
      break;
    }
    nodes.push_back(std::move(node));
  }
}

void ObjParser::parse() {
  MeshNode mesh;
  while (cursor < tokens.size()) {
    Token token = opt();

    if (token.type == TokenType::ENDOFFILE) {
      break;
    } else if (token.type == TokenType::OBJECT) {
      consume();
      mesh.mesh_name = std::string(opt().lexeme);
      consume();
      make_mesh();
      mesh.nodes = std::move(nodes);
      nodes.clear();
    } else if (token.type == TokenType::MTLLIB) {
      consume();
      mtl_labels.push_back(std::make_unique<MtlLibNode>(make_mtllib()));
      continue;
    } else {
      make_mesh();
      mesh.mesh_name = "Object";
      mesh.nodes = std::move(nodes);
      nodes.clear();
    }
    meshes.push_back(std::make_unique<MeshNode>(mesh));
  }
}

} // namespace obj_parser
} // namespace tracer