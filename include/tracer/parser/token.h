#pragma once
#include <string>
#include <string_view>

namespace tracer {

namespace parser {

enum class TokenType {
  Identifier,
  Number,

  LeftParen,
  RightParen,

  LeftBrace,
  RightBrace,

  Colon,
  Comma,

  Newline,
  EndOfFile
};

struct Token {
  Token() = default;
  Token(TokenType type, std::string_view lexeme)
      : type(type), lexeme(lexeme) {};

  TokenType type;
  std::string_view lexeme;
};

} // namespace parser

} // namespace tracer