#pragma once

#include "ast.h"
#include "exceptions.h"
#include "token.h"
#include <stdexcept>


namespace tracer {

namespace parser {

class Parser {
public:
  Parser(const std::vector<Token> &tokens) : tokens(tokens) {}
  ~Parser() = default;

  void parse();

  const SceneAST &get_ast() const { return ast; }

private:
  std::vector<Token> tokens;
  size_t cursor = 0;
  SceneAST ast;

  void parse_property(PropertyNode &prop);
  void parse_number(PropertyNode &prop, const std::string_view &lexeme);
  void parse_pair(PropertyNode &prop);
  void parse_vector3(PropertyNode &prop);
  void parse_quaternion(PropertyNode &prop);

  Token peek_token() const;
  Token expect_token(TokenType expected);
  Token next_token();
};

} // namespace parser

} // namespace tracer