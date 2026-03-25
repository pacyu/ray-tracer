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

  const Auroric &get_ast() const { return ast; }

private:
  std::vector<Token> tokens;
  size_t cursor = 0;
  Auroric ast;

  Statement parse_statement();
  std::shared_ptr<ASTNode> parse_expression(const Precedence &);
  std::shared_ptr<ASTNode> parse_lambda(const std::string_view &);
  std::shared_ptr<ASTNode> parse_variable(const std::string_view &);
  std::shared_ptr<ASTNode> parse_number(const std::string_view &);
  std::shared_ptr<ASTNode> parse_pair();
  std::shared_ptr<ASTNode> parse_vector3();
  std::shared_ptr<ASTNode> parse_quaternion();
  std::shared_ptr<ASTNode> parse_random();
  std::shared_ptr<ASTNode> parse_string(const std::string_view &);
  std::shared_ptr<ASTNode> parse_list();
  std::shared_ptr<ASTNode> parse_paren_list();
  std::shared_ptr<ASTNode> parse_lambertian();
  std::shared_ptr<ASTNode> parse_metal();
  std::shared_ptr<ASTNode> parse_dielectric();
  std::shared_ptr<ASTNode> parse_light();
  std::shared_ptr<ASTNode> parse_xyrect();
  std::shared_ptr<ASTNode> parse_xzrect();
  std::shared_ptr<ASTNode> parse_yzrect();
  std::shared_ptr<ASTNode> parse_box();
  std::shared_ptr<ASTNode> parse_sphere();
  std::shared_ptr<ASTNode> parse_heart();
  std::shared_ptr<ASTNode> parse_translate();
  std::shared_ptr<ASTNode> parse_constant_medium();
  std::shared_ptr<ASTNode> parse_camera();

  char get_operator(const TokenType &) const;
  Precedence get_precedence(const TokenType &) const;
  bool next_token_is(const TokenType &) const;
  Token peek_token() const;
  Token expect_token(const std::vector<TokenType> &);
  Token advance();
};

} // namespace parser

} // namespace tracer