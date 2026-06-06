#pragma once
#include "tracer/obj_parser/token.h"
#include <iostream>
#include <unordered_map>
#include <vector>

namespace tracer {
namespace obj_parser {

class Lexer {
public:
  Lexer(const std::string &src);
  ~Lexer() = default;

  std::vector<Token> tokenizer();

private:
  std::string_view source;
  size_t cursor = 0;
  std::unordered_map<std::string, Token> reserved;

  void skip_whitespaces_and_comments();
  char take();
  char peek();
  void consume();
  Token peek_number();
  Token peek_identifier();
};

} // namespace obj_parser
} // namespace tracer