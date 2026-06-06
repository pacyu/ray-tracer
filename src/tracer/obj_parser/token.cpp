#include "tracer/obj_parser/token.h"

namespace tracer {
namespace obj_parser {

std::string get_type(TokenType type) {
  switch (type) {
  case TokenType::NUMBER:
    return "number";
  case TokenType::VERTEX:
    return "vertex";
  case TokenType::TEXTURE_COORD:
    return "texture coordinate";
  case TokenType::NORMAL:
    return "normal";
  case TokenType::FACE:
    return "face";
  case TokenType::OBJECT:
    return "object";
  case TokenType::GROUP:
    return "group";
  case TokenType::USEMTL:
    return "usemtl";
  case TokenType::MTLLIB:
    return "mtllib";
  case TokenType::SMOOTH:
    return "smooth";
  case TokenType::LINE:
    return "line";
  case TokenType::POINT:
    return "point";
  case TokenType::SLASH:
    return "/";
  case TokenType::NEWMTL:
    return "newmtl";
  case TokenType::AMBIENT:
    return "ambient";
  case TokenType::DIFFUSE:
    return "diffuse";
  case TokenType::SPECULAR:
    return "specular";
  case TokenType::SHININESS:
    return "shininess";
  case TokenType::OPACITY:
    return "opacity";
  case TokenType::TRANSPARENCY:
    return "transparency";
  case TokenType::MAP_KD:
    return "map_kd";
  default:
    return "Unknown type";
  }
}

} // namespace obj_parser
} // namespace tracer