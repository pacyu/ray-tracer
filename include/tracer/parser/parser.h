#pragma once
#include "tracer/parser/ast.h"
#include "tracer/parser/exceptions.h"
#include "tracer/parser/token.h"
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

  InstanceNode parse_named_instance();
  InstanceNode parse_anonymous_instance();
  InstanceNode parse_expression_statement();
  void parse_type_definition(InstanceNode &instance);
  void parse_type_instance(InstanceNode &instance);
  void parse_value(Argument &argument);
  void parse_arguments(std::vector<Argument> &arguments);
  void parse_pair(Argument &prop);
  void parse_vector3(Argument &prop);
  void parse_quaternion(Argument &prop);
  void parse_number(Argument &prop, const std::string_view &lexeme);

  bool next_token_is(TokenType type) const;
  Token peek_token() const;
  Token expect_token(size_t mask);
  Token expect_token(TokenType type);
  Token next_token();
};

} // namespace parser

} // namespace tracer