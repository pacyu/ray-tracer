#pragma once
#include <string>
#include <string_view>

namespace tracer {
namespace obj_parser {

enum class TokenType {
  IDENTIFIER,
  VERTEX,
  NORMAL,
  TEXTURE_COORD,
  FACE,
  OBJECT,
  GROUP,
  USEMTL,
  MTLLIB,
  SMOOTH,
  LINE,
  POINT,
  NUMBER,
  SLASH,
  NEWLINE,

  NEWMTL,
  AMBIENT,
  DIFFUSE,
  SPECULAR,
  SHININESS,
  OPACITY,
  TRANSPARENCY,

  MAP_KD,
  MAP_KS,
  MAP_KA,
  MAP_NS,
  MAP_D,
  MAP_BUMP,
  MAP_KE,
  REFL,

  ENDOFFILE
};

struct Token {
  Token() = default;
  Token(TokenType type, std::string_view lexeme)
      : type(type), lexeme(lexeme) {};

  TokenType type;
  std::string_view lexeme;
};

std::string get_type(TokenType);

} // namespace obj_parser
} // namespace tracer
