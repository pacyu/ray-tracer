#include "tracer/parser/parser.h"

namespace tracer {

namespace parser {

bool Parser::next_token_is(TokenType type) const {
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

Token Parser::expect_token(TokenType type) {
  return expect_token(static_cast<size_t>(type));
}

Token Parser::expect_token(size_t mask) {
  Token t = peek_token();
  size_t bit = static_cast<size_t>(t.type);
  if (bit & mask) {
    return tokens[cursor++];
  }
  throw ParseException("Expected " + get_expected_types_string(mask) +
                       "but found" + to_string(t.type));
}

Token Parser::next_token() {
  ++cursor;
  if (cursor < tokens.size()) {
    return tokens[cursor];
  }
  return Token(TokenType::EndOfFile, "");
}

void Parser::parse_number(Argument &argument, const std::string_view &lexeme) {
  try {
    if (lexeme.find('.') != std::string_view::npos) {
      argument.type = Argument::FLOAT;
      argument.value = std::stof(std::string(lexeme));
    } else {
      argument.type = Argument::INT;
      argument.value = std::stoi(std::string(lexeme));
    }
  } catch (const std::exception &e) {
    throw ParseException("Invalid number format: " + std::string(lexeme));
  }
}

void Parser::parse_pair(Argument &argument) {
  size_t cursor_backup = cursor;

  try {
    Token first = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token second = expect_token(TokenType::Number);
    expect_token(static_cast<size_t>(TokenType::RightBracket) |
                 static_cast<size_t>(TokenType::RightParen));

    int x = std::stoi(std::string(first.lexeme));
    int y = std::stoi(std::string(second.lexeme));

    argument.type = Argument::PAIR;
    argument.value = Pair{x, y};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParsePairException(
        "Pair must consist of two integers separated by a comma.");
  }
}

void Parser::parse_vector3(Argument &argument) {
  size_t cursor_backup = cursor;

  try {
    Token x = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token y = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token z = expect_token(TokenType::Number);
    expect_token(static_cast<size_t>(TokenType::RightBracket) |
                 static_cast<size_t>(TokenType::RightParen));

    argument.type = Argument::VECTOR3;
    argument.value =
        Vec3{std::stof(std::string(x.lexeme)), std::stof(std::string(y.lexeme)),
             std::stof(std::string(z.lexeme))};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParseVector3Exception(
        "Vector3 must consist of three numbers separated by commas.");
  }
}

void Parser::parse_quaternion(Argument &argument) {
  size_t cursor_backup = cursor;

  try {
    Token x = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token y = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token z = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token w = expect_token(TokenType::Number);
    expect_token(static_cast<size_t>(TokenType::RightBracket) |
                 static_cast<size_t>(TokenType::RightParen));

    argument.type = Argument::QUATERNION;
    argument.value = Quaternion{
        std::stof(std::string(x.lexeme)), std::stof(std::string(y.lexeme)),
        std::stof(std::string(z.lexeme)), std::stof(std::string(w.lexeme))};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParseQuaternionException(
        "Quaternion must consist of four numbers separated by commas.");
  }
}

void Parser::parse_value(Argument &argument) {
  Token value = peek_token();

  switch (value.type) {
  case TokenType::Number:
    parse_number(argument, value.lexeme);
    next_token();
    break;
  case TokenType::LeftBracket:
    next_token();
    try {
      parse_pair(argument);
    } catch (const ParsePairException &e) {
      try {
        parse_vector3(argument);
      } catch (const ParseVector3Exception &e) {
        try {
          parse_quaternion(argument);
        } catch (const ParseQuaternionException &e) {
          throw ParseException("Invalid argument value for " +
                               std::string(argument.name));
        }
      }
    }
    break;
  case TokenType::LeftParen:
    next_token();
    try {
      parse_pair(argument);
    } catch (const ParsePairException &e) {
      try {
        parse_vector3(argument);
      } catch (const ParseVector3Exception &e) {
        try {
          parse_quaternion(argument);
        } catch (const ParseQuaternionException &e) {
          throw ParseException("Invalid argument value for " +
                               std::string(argument.name));
        }
      }
    }
    break;
  case TokenType::Identifier:
    argument.type = Argument::STRING;
    argument.value = std::string(value.lexeme);
    next_token();
    break;
  case TokenType::Quote:
    value = next_token();
    next_token();
    argument.type = Argument::STRING;
    argument.value = std::string(value.lexeme);
    expect_token(TokenType::Quote);
    break;
  default:
    throw ParseException("Unexpected token type: " + to_string(value.type));
  }
}

void Parser::parse_arguments(std::vector<Argument> &arguments) {
  while ((peek_token().type != TokenType::RightParen) &&
         (peek_token().type != TokenType::RightBrace)) {
    Token property = expect_token(static_cast<size_t>(TokenType::Identifier) |
                                  static_cast<size_t>(TokenType::Comma));
    Argument argument;

    if (property.type == TokenType::Comma) {
      property = expect_token(TokenType::Identifier);
    }
    argument.name = property.lexeme;
    expect_token(TokenType::Colon);
    parse_value(argument);
    arguments.push_back(argument);
  }
}

void Parser::parse_type_definition(InstanceNode &instance) {
  Token token = expect_token(TokenType::Identifier);
  instance._typename = token.lexeme;
  parse_value(instance.arg);
}

void Parser::parse_type_instance(InstanceNode &instance) {
  Token token = expect_token(TokenType::Identifier);
  instance._typename = token.lexeme;
  expect_token(static_cast<size_t>(TokenType::LeftBrace) |
               static_cast<size_t>(TokenType::LeftParen));
  parse_arguments(instance.args);
  expect_token(static_cast<size_t>(TokenType::RightBrace) |
               static_cast<size_t>(TokenType::RightParen));
}

InstanceNode Parser::parse_expression_statement() {
  InstanceNode instance;
  instance.kind = InstanceNode::EXPRESSION;

  Token token = expect_token(TokenType::Identifier);
  instance.varname = token.lexeme;

  expect_token(TokenType::Equal);
  parse_type_definition(instance);
  return instance;
}

InstanceNode Parser::parse_anonymous_instance() {
  InstanceNode instance;
  instance.kind = InstanceNode::ANONYMOUS;
  parse_type_instance(instance);
  return instance;
}

InstanceNode Parser::parse_named_instance() {
  InstanceNode instance;
  instance.kind = InstanceNode::NAMED;

  Token token = expect_token(TokenType::Identifier);
  instance.varname = token.lexeme;

  expect_token(TokenType::Colon);
  parse_type_instance(instance);
  return instance;
}

void Parser::parse() {
  while (peek_token().type != TokenType::EndOfFile) {
    InstanceNode instance;
    if ((peek_token().type == TokenType::Identifier) &&
        next_token_is(TokenType::Colon)) {
      instance = parse_named_instance();
    } else if ((peek_token().type == TokenType::Identifier) &&
               next_token_is(TokenType::Equal)) {
      instance = parse_expression_statement();
    } else {
      instance = parse_anonymous_instance();
    }

    ast.instances.push_back(instance);
  }
}

} // namespace parser

} // namespace tracer