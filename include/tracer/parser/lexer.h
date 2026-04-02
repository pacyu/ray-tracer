#pragma once
#include "tracer/parser/token.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace tracer {
namespace parser {

class Lexer {
public:
  Lexer(const std::string &src);
  ~Lexer() = default;

  std::vector<Token> tokenizer();

private:
  std::string_view source;
  std::unordered_map<std::string, TokenType> reserved;
  size_t cursor = 0;

  char peek() const;
  char lookahead() const;
  void advance();
  void skip_whitespaces_and_comments();
  Token peek_ddot();
  Token peek_leftarrow();
  Token peek_rightarrow();
  Token peek_number();
  Token peek_string();
  Token peek_identifier();
  Token peek_char();
  Token next_token();
};

} // namespace parser
} // namespace tracer