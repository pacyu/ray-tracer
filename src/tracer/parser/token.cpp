#include "tracer/parser/token.h"

namespace tracer {
namespace parser {

std::string to_string(TokenType type) {
  switch (type) {
  case TokenType::Identifier:
    return "Identifier";
  case TokenType::Number:
    return "Number";
  case TokenType::NumberType:
    return "Num";
  case TokenType::Pair:
    return "Pair";
  case TokenType::Vector3:
    return "Vec3";
  case TokenType::Quater:
    return "Quater";
  case TokenType::List:
    return "List";
  case TokenType::String:
    return "String";
  case TokenType::Random:
    return "Random";
  case TokenType::LambertianType:
    return "Lambert";
  case TokenType::MetalType:
    return "Metal";
  case TokenType::DielectricType:
    return "Dielect";
  case TokenType::LightType:
    return "Light";
  case TokenType::XYRectType:
    return "XYRect";
  case TokenType::XZRectType:
    return "XZRect";
  case TokenType::YZRectType:
    return "YZRect";
  case TokenType::BoxType:
    return "Box";
  case TokenType::SphereType:
    return "Sphere";
  case TokenType::HeartType:
    return "Heart";
  case TokenType::TranslateType:
    return "Translate";
  case TokenType::ConstantMediumType:
    return "ConstMedium";
  case TokenType::CameraType:
    return "Camera";
  case TokenType::LeftBrace:
    return "LeftBrace '{'";
  case TokenType::LeftBracket:
    return "LeftBracket '['";
  case TokenType::LeftParen:
    return "LeftParen '('";
  case TokenType::RightBrace:
    return "RightBrace '}'";
  case TokenType::RightBracket:
    return "RightBracket ']'";
  case TokenType::RightParen:
    return "RightParen ')'";
  case TokenType::LeftArrow:
    return "LeftArrow '<-'";
  case TokenType::RightArrow:
    return "RightArrow '->'";
  case TokenType::Colon:
    return "Colon ':'";
  case TokenType::Comma:
    return "Comma ','";
  case TokenType::Equal:
    return "Equal '='";
  case TokenType::Pipe:
    return "Pipe '|'";
  case TokenType::Plus:
    return "Plus '+'";
  case TokenType::Minus:
    return "Minus '-'";
  case TokenType::Mul:
    return "Mul '*'";
  case TokenType::Div:
    return "Div '/'";
  case TokenType::Mod:
    return "Mod '%'";
  case TokenType::Quote:
    return "Quote '\''";
  case TokenType::DoubleQuote:
    return "DoubleQuote '\"'";
  case TokenType::DDot:
    return "DDot '..'";
  case TokenType::LiteralStr:
    return "LiteralStr";
  case TokenType::Newline:
    return "Newline '\n'";
  default:
    return "UnknownToken";
  }
}

std::string get_expected_types_string(std::vector<TokenType> types) {
  std::vector<std::string> expected;

  for (const auto &type : types)
    expected.push_back(to_string(type));

  std::stringstream ss;
  for (size_t i = 0; i < expected.size(); ++i) {
    ss << expected[i];
    if (i < expected.size() - 2)
      ss << ", ";
    else if (i == expected.size() - 2)
      ss << " or ";
  }
  return ss.str();
}

} // namespace parser
} // namespace tracer