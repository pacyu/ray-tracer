#pragma once
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace tracer {

namespace parser {

enum class TokenType {
  Identifier,
  Number,
  NumberType,
  Pair,
  Vector3,
  Quater,
  List,
  String,
  Random,
  LambertianType,
  MetalType,
  DielectricType,
  LightType,
  XYRectType,
  XZRectType,
  YZRectType,
  BoxType,
  SphereType,
  HeartType,
  TranslateType,
  ConstantMediumType,
  CameraType,

  LeftParen,
  RightParen,

  LeftBracket,
  RightBracket,

  LeftBrace,
  RightBrace,

  LeftArrow,
  RightArrow,

  Colon,
  Comma,
  Equal,
  Pipe,
  Quote,
  DoubleQuote,

  DDot,

  Plus,
  Minus,
  Mul,
  Div,
  Mod,

  LiteralStr,

  Newline,
  EndOfFile
};

enum Precedence {
  NONE,
  ASSIGN,     // =
  R_ARROW,    // ->
  TERM,       // + -
  FACTOR,     // * /
  UNARY,      // - !
  CALL,       // | (Pipe)
  PRIMARY
};

struct Token {
  Token() = default;
  Token(TokenType type, std::string_view lexeme)
      : type(type), lexeme(lexeme) {};

  TokenType type;
  std::string_view lexeme;
};

std::string to_string(TokenType type);
std::string get_expected_types_string(std::vector<TokenType> types);

} // namespace parser

} // namespace tracer