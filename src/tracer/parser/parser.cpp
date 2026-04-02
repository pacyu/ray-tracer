#include "tracer/parser/parser.h"

namespace tracer {
namespace parser {

char Parser::get_operator(const TokenType &type) const {
  switch (type) {
  case TokenType::Plus:
    return '+';
  case TokenType::Minus:
    return '-';
  case TokenType::Mul:
    return '*';
  case TokenType::Div:
    return '/';
  case TokenType::Pipe:
    return '|';
  default:
    throw ParseException("Unknown operator: " + to_string(type));
  }
}

Precedence Parser::get_precedence(const TokenType &type) const {
  switch (type) {
  case TokenType::Equal:
    return Precedence::ASSIGN;
  case TokenType::RightArrow:
    return Precedence::R_ARROW;
  case TokenType::Plus:
    return Precedence::TERM;
  case TokenType::Minus:
    return Precedence::TERM;
  case TokenType::Mul:
    return Precedence::FACTOR;
  case TokenType::Div:
    return Precedence::FACTOR;
  case TokenType::Pipe:
    return Precedence::CALL;
  default:
    return Precedence::NONE;
  }
}

bool Parser::next_token_is(const TokenType &type) const {
  if (cursor + 1 < tokens.size())
    return tokens[cursor + 1].type == type;
  return false;
}

Token Parser::peek_token() const {
  if (cursor < tokens.size()) {
    return tokens[cursor];
  }
  return Token(TokenType::EndOfFile, "");
}

Token Parser::expect_token(const std::vector<TokenType> &masks) {
  Token t = peek_token();
  if (std::find(masks.begin(), masks.end(), t.type) != masks.end()) {
    return tokens[cursor++];
  }
  throw ParseException("Expected " + get_expected_types_string(masks) +
                       "but found" + to_string(t.type));
}

Token Parser::advance() {
  ++cursor;
  if (cursor < tokens.size()) {
    return tokens[cursor];
  }
  return Token(TokenType::EndOfFile, "");
}

std::shared_ptr<ASTNode> Parser::parse_number(const std::string_view &lexeme) {
  try {
    if (lexeme.find('.') != std::string::npos) {
      float val = std::stof(std::string(lexeme));
      return std::make_shared<LiteralNode>(val);
    } else {
      int val = std::stoi(std::string(lexeme));
      return std::make_shared<LiteralNode>(val);
    }
  } catch (const std::exception &) {
    throw ParseException("Invalid number format: " + std::string(lexeme));
  }
}

std::shared_ptr<ASTNode> Parser::parse_lambda(const std::string_view &name) {
  Token token = expect_token({TokenType::RightArrow});
  std::shared_ptr<ASTNode> body = parse_expression(Precedence::NONE);
  return std::make_shared<LambdaNode>(std::string(name), body);
}

std::shared_ptr<ASTNode> Parser::parse_variable(const std::string_view &name) {
  return std::make_shared<VariableNode>(std::string(name));
}

std::shared_ptr<ASTNode> Parser::parse_pair() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> first = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> second = parse_expression(Precedence::NONE);
  expect_token({TokenType::RightParen});
  return std::make_shared<PairNode>(first, second);
}

std::shared_ptr<ASTNode> Parser::parse_vector3() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> x = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> y = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> z = parse_expression(Precedence::NONE);
  expect_token({TokenType::RightParen});
  return std::make_shared<Vec3Node>(x, y, z);
}

std::shared_ptr<ASTNode> Parser::parse_quaternion() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> x = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> y = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> z = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> w = parse_expression(Precedence::NONE);
  expect_token({TokenType::RightParen});
  return std::make_shared<QuaterNode>(x, y, z, w);
}

std::shared_ptr<ASTNode> Parser::parse_random() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> first = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> second = parse_expression(Precedence::NONE);
  expect_token({TokenType::RightParen});
  return std::make_shared<RandomNode>(first, second);
}

std::shared_ptr<ASTNode> Parser::parse_string(const std::string_view &value) {
  return std::make_shared<LiteralNode>(std::string(value));
}

std::shared_ptr<ASTNode> Parser::parse_list() {
  std::shared_ptr<ASTNode> begin = parse_expression(Precedence::NONE);

  if (peek_token().type == TokenType::DDot) {
    advance();
    std::shared_ptr<ASTNode> end = parse_expression(Precedence::NONE);
    if (peek_token().type == TokenType::Colon) {
      advance();
      std::shared_ptr<ASTNode> step = parse_expression(Precedence::NONE);
      expect_token({TokenType::RightBracket});
      return std::make_shared<ListNode>(begin, end, step);
    }
    expect_token({TokenType::RightBracket});
    return std::make_shared<ListNode>(begin, end);
  } else {
    std::vector<std::shared_ptr<ASTNode>> elements;
    elements.push_back(begin);
    while (peek_token().type != TokenType::RightBracket) {
      elements.push_back(parse_expression(Precedence::NONE));
    }
    expect_token({TokenType::RightBracket});
    return std::make_shared<ListNode>(elements);
  }
}

std::shared_ptr<ASTNode> Parser::parse_paren_list() {
  std::vector<std::shared_ptr<ASTNode>> list;
  while (peek_token().type != TokenType::RightParen) {
    list.push_back(parse_expression(Precedence::NONE));
  }
  expect_token({TokenType::RightParen});
  if (list.size() == 1) {
    return list[0];
  } else if (list.size() == 2) {
    return std::make_shared<PairNode>(list[0], list[1]);
  } else if (list.size() == 3) {
    return std::make_shared<Vec3Node>(list[0], list[1], list[2]);
  } else if (list.size() == 4) {
    return std::make_shared<QuaterNode>(list[0], list[1], list[2], list[3]);
  }
  throw std::runtime_error("Unsupported type of the parenthesis expression.");
}

std::shared_ptr<ASTNode> Parser::parse_lambertian() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> albedo = parse_expression(Precedence::NONE);
  std::shared_ptr<LambertianNode> lambertian =
      std::make_shared<LambertianNode>(albedo);
  expect_token({TokenType::RightParen});
  return lambertian;
}

std::shared_ptr<ASTNode> Parser::parse_metal() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> albedo = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> fuzz = parse_expression(Precedence::NONE);
  std::shared_ptr<MetalNode> metal = std::make_shared<MetalNode>(albedo, fuzz);
  expect_token({TokenType::RightParen});
  return metal;
}

std::shared_ptr<ASTNode> Parser::parse_dielectric() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> albedo = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> density = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> ri = parse_expression(Precedence::NONE);
  std::shared_ptr<DielectricNode> dielectric =
      std::make_shared<DielectricNode>(albedo, density, ri);
  expect_token({TokenType::RightParen});
  return dielectric;
}

std::shared_ptr<ASTNode> Parser::parse_ocean_material() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> ior = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> roughness = parse_expression(Precedence::NONE);
  std::shared_ptr<OceanMaterialNode> ocean_mat =
      std::make_shared<OceanMaterialNode>(ior, roughness);
  expect_token({TokenType::RightParen});
  return ocean_mat;
}

std::shared_ptr<ASTNode> Parser::parse_light() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> albedo = parse_expression(Precedence::NONE);
  std::shared_ptr<DiffuseLightNode> light =
      std::make_shared<DiffuseLightNode>(albedo);
  expect_token({TokenType::RightParen});
  return light;
}

std::shared_ptr<ASTNode> Parser::parse_xyrect() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> quater = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> k = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<XYRectNode> xyrect =
      std::make_shared<XYRectNode>(quater, k, mat);
  expect_token({TokenType::RightParen});
  return xyrect;
}

std::shared_ptr<ASTNode> Parser::parse_xzrect() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> quater = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> k = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<XZRectNode> xzrect =
      std::make_shared<XZRectNode>(quater, k, mat);
  expect_token({TokenType::RightParen});
  return xzrect;
}

std::shared_ptr<ASTNode> Parser::parse_yzrect() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> quater = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> k = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<YZRectNode> yzrect =
      std::make_shared<YZRectNode>(quater, k, mat);
  expect_token({TokenType::RightParen});
  return yzrect;
}

std::shared_ptr<ASTNode> Parser::parse_box() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> bmin = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> bmax = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);

  std::shared_ptr<BoxNode> box = nullptr;
  if (peek_token().type != TokenType::RightParen) {
    std::shared_ptr<ASTNode> rot = parse_expression(Precedence::NONE);
    box = std::make_shared<BoxNode>(bmin, bmax, mat, rot);
  } else {
    box = std::make_shared<BoxNode>(bmin, bmax, mat, nullptr);
  }
  expect_token({TokenType::RightParen});
  return box;
}

std::shared_ptr<ASTNode> Parser::parse_sphere() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> center = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> radius = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<SphereNode> sphere =
      std::make_shared<SphereNode>(center, radius, mat);
  expect_token({TokenType::RightParen});
  return sphere;
}

std::shared_ptr<ASTNode> Parser::parse_heart() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> center = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> rho = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<HeartNode> heart = nullptr;
  if (peek_token().type != TokenType::RightParen) {
    std::shared_ptr<ASTNode> rot = parse_expression(Precedence::NONE);
    heart = std::make_shared<HeartNode>(center, rho, mat, rot);
  } else {
    heart = std::make_shared<HeartNode>(center, rho, mat, nullptr);
  }
  expect_token({TokenType::RightParen});
  return heart;
}

std::shared_ptr<ASTNode> Parser::parse_ocean() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> n = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> l = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> wind_speed = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> wind_dir = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> a = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> mat = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> lam = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> height = parse_expression(Precedence::NONE);
  std::shared_ptr<OceanNode> ocean = std::make_shared<OceanNode>(
      n, l, wind_speed, wind_dir, a, mat, lam, height);
  expect_token({TokenType::RightParen});
  return ocean;
}

std::shared_ptr<ASTNode> Parser::parse_translate() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> object = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> pos = parse_expression(Precedence::NONE);
  std::shared_ptr<TranslateNode> translate =
      std::make_shared<TranslateNode>(object, pos);
  expect_token({TokenType::RightParen});
  return translate;
}

std::shared_ptr<ASTNode> Parser::parse_constant_medium() {
  expect_token({TokenType::LeftParen});
  std::shared_ptr<ASTNode> albedo = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> density = parse_expression(Precedence::NONE);
  std::shared_ptr<ASTNode> object = parse_expression(Precedence::NONE);
  std::shared_ptr<ConstantMediumNode> cm =
      std::make_shared<ConstantMediumNode>(albedo, density, object);
  expect_token({TokenType::RightParen});
  return cm;
}

std::shared_ptr<ASTNode> Parser::parse_camera() {
  expect_token({TokenType::LeftParen});
  std::vector<std::shared_ptr<ASTNode>> args;

  do {
    args.push_back(parse_expression(Precedence::NONE));
  } while (peek_token().type != TokenType::RightParen);

  expect_token({TokenType::RightParen});

  std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>();
  if (args.size() > 0)
    camera->shape_expr = args[0];
  if (args.size() > 1)
    camera->spp_expr = args[1];
  if (args.size() > 2)
    camera->depth_expr = args[2];
  if (args.size() > 3)
    camera->name_expr = args[3];
  if (args.size() > 4)
    camera->bg_expr = args[4];
  if (args.size() > 5)
    camera->from_expr = args[5];
  if (args.size() > 6)
    camera->at_expr = args[6];
  if (args.size() > 7)
    camera->vup_expr = args[7];
  if (args.size() > 8)
    camera->fov_expr = args[8];
  if (args.size() > 9)
    camera->fov_expr = args[9];

  return camera;
}

std::shared_ptr<ASTNode> Parser::parse_expression(const Precedence &prec) {
  Token token = peek_token();
  std::shared_ptr<ASTNode> left = nullptr;
  advance();

  switch (token.type) {

  case TokenType::Comma: {
    left = parse_expression(Precedence::NONE);
    break;
  }

  case TokenType::Minus: {
    std::shared_ptr<ASTNode> operand = parse_expression(Precedence::UNARY);
    left = std::make_shared<UnaryOpNode>('-', operand);
    break;
  }

  case TokenType::LeftParen: {
    left = parse_paren_list();
    break;
  }

  case TokenType::LeftBracket: {
    left = parse_list();
    break;
  }

  case TokenType::Identifier: {
    if (peek_token().type == TokenType::RightArrow)
      left = parse_lambda(token.lexeme);
    else
      left = parse_variable(token.lexeme);
    break;
  }

  case TokenType::NumberType: {
    left = parse_expression(Precedence::NONE);
    break;
  }

  case TokenType::Number: {
    left = parse_number(token.lexeme);
    break;
  }

  case TokenType::String: {
    left = parse_expression(Precedence::NONE);
    break;
  }

  case TokenType::LiteralStr: {
    left = parse_string(token.lexeme);
    break;
  }

  case TokenType::List: {
    left = parse_expression(Precedence::NONE);
    break;
  }

  case TokenType::Pair: {
    left = parse_pair();
    break;
  }

  case TokenType::Vector3: {
    left = parse_vector3();
    break;
  }

  case TokenType::Quater: {
    left = parse_quaternion();
    break;
  }

  case TokenType::Random: {
    left = parse_random();
    break;
  }

  case TokenType::LambertianType: {
    left = parse_lambertian();
    break;
  }

  case TokenType::MetalType: {
    left = parse_metal();
    break;
  }

  case TokenType::DielectricType: {
    left = parse_dielectric();
    break;
  }

  case TokenType::LightType: {
    left = parse_light();
    break;
  }

  case TokenType::TranslateType: {
    left = parse_translate();
    break;
  }

  case TokenType::XYRectType: {
    left = parse_xyrect();
    break;
  }

  case TokenType::XZRectType: {
    left = parse_xzrect();
    break;
  }

  case TokenType::YZRectType: {
    left = parse_yzrect();
    break;
  }

  case TokenType::BoxType: {
    left = parse_box();
    break;
  }

  case TokenType::SphereType: {
    left = parse_sphere();
    break;
  }

  case TokenType::HeartType: {
    left = parse_heart();
    break;
  }

  case TokenType::OceanType: {
    left = parse_ocean();
    break;
  }

  case TokenType::ConstantMediumType: {
    left = parse_constant_medium();
    break;
  }

  case TokenType::CameraType: {
    left = parse_camera();
  }

  default:
    throw ParseException("Unknown token type: " + to_string(token.type));
  }

  while (prec < get_precedence(peek_token().type)) {
    Token op = peek_token();
    advance();
    if (op.type == TokenType::Plus || op.type == TokenType::Minus ||
        op.type == TokenType::Mul || op.type == TokenType::Div) {
      std::shared_ptr<ASTNode> right =
          parse_expression(get_precedence(op.type));
      left = std::make_shared<BinaryOpNode>(get_operator(op.type), left, right);
    } else if (op.type == TokenType::Pipe) {
      std::shared_ptr<ASTNode> right = parse_expression(Precedence::CALL);
      left = std::make_shared<PipeNode>(get_operator(op.type), left, right);
    }
  }
  return left;
}

Statement Parser::parse_statement() {
  Token variant = expect_token(
      {TokenType::Identifier, TokenType::XYRectType, TokenType::XZRectType,
       TokenType::YZRectType, TokenType::BoxType, TokenType::SphereType,
       TokenType::HeartType, TokenType::TranslateType,
       TokenType::ConstantMediumType, TokenType::CameraType});
  Token token = expect_token({TokenType::Equal, TokenType::LeftParen});
  std::shared_ptr<ASTNode> expr = nullptr;
  std::string var_name = std::string(variant.lexeme);
  switch (token.type) {
  case TokenType::Equal:
    expr = parse_expression(Precedence::NONE);
    break;
  case TokenType::LeftParen:
    cursor -= 2;
    var_name = "anonymous_" + var_name + std::to_string(cursor);
    expr = parse_expression(Precedence::NONE);
    break;
  default:
    break;
  }
  return Statement(std::string(var_name), expr);
}

void Parser::parse() {
  while (peek_token().type != TokenType::EndOfFile) {
    Statement statement;
    statement = parse_statement();
    ast.program.push_back(statement);
  }
}

} // namespace parser
} // namespace tracer