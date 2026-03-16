#pragma once
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace tracer {

namespace parser {

enum class TokenType {
  Identifier   = 1 << 0,
  Number       = 1 << 1,

  Quote        = 1 << 2,
  LeftParen    = 1 << 3,
  RightParen   = 1 << 4,

  LeftBracket  = 1 << 5,
  RightBracket = 1 << 6,

  LeftBrace    = 1 << 7,
  RightBrace   = 1 << 8,

  Colon        = 1 << 9,
  Comma        = 1 << 10,
  Equal        = 1 << 11,

  Newline      = 1 << 12,
  EndOfFile    = 1 << 13
};

struct Token {
  Token() = default;
  Token(TokenType type, std::string_view lexeme)
      : type(type), lexeme(lexeme) {};

  TokenType type;
  std::string_view lexeme;
};

std::string to_string(TokenType type);
std::string get_expected_types_string(size_t mask);

} // namespace parser

} // namespace tracer