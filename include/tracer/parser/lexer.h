#pragma once
#include "token.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace tracer {

namespace parser {

class Lexer {
public:
  Lexer(const std::string &src) : source(src) {}
  ~Lexer() = default;

  std::vector<Token> tokenizer();

private:
  std::string_view source;
  size_t cursor = 0;

  char peek() const;
  void advance();
  void skip_whitespaces_and_comments();
  Token peek_number();
  Token peek_identifier();
  Token peek_char();
  Token next_token();
};

} // namespace parser

} // namespace tracer