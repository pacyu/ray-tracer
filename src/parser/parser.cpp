#include "tracer/parser/parser.h"

namespace tracer {

namespace parser {

Token Parser::peek_token() const {
  if (cursor < tokens.size()) {
    return tokens[cursor];
  }
  return Token(TokenType::EndOfFile, "");
}

Token Parser::expect_token(TokenType expected) {
  if (tokens[cursor].type == expected) {
    return tokens[cursor++];
  }
  throw ParseException("Expected token type: " +
                       std::to_string(static_cast<int>(expected)));
}

Token Parser::next_token() {
  ++cursor;
  if (cursor < tokens.size()) {
    return tokens[cursor];
  }
  return Token(TokenType::EndOfFile, "");
}

void Parser::parse_number(PropertyNode &prop, const std::string_view &lexeme) {
  try {
    if (lexeme.find('.') != std::string_view::npos) {
      prop.value = std::stof(std::string(lexeme));
    } else {
      prop.value = std::stoi(std::string(lexeme));
    }
  } catch (const std::exception &e) {
    throw ParseException("Invalid number format: " + std::string(lexeme));
  }
}

void Parser::parse_pair(PropertyNode &prop) {
  size_t cursor_backup = cursor;

  try {
    Token first = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token second = expect_token(TokenType::Number);
    expect_token(TokenType::RightParen);

    int x = std::stoi(std::string(first.lexeme));
    int y = std::stoi(std::string(second.lexeme));

    prop.value = pair{x, y};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParsePairException(
        "Pair must consist of two integers separated by a comma.");
  }
}

void Parser::parse_vector3(PropertyNode &prop) {
  size_t cursor_backup = cursor;

  try {
    Token x = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token y = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token z = expect_token(TokenType::Number);
    expect_token(TokenType::RightParen);

    prop.value =
        Vec3{std::stof(std::string(x.lexeme)), std::stof(std::string(y.lexeme)),
             std::stof(std::string(z.lexeme))};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParseVector3Exception(
        "Vector3 must consist of three numbers separated by commas.");
  }
}

void Parser::parse_quaternion(PropertyNode &prop) {
  size_t cursor_backup = cursor;

  try {
    Token x = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token y = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token z = expect_token(TokenType::Number);
    expect_token(TokenType::Comma);
    Token w = expect_token(TokenType::Number);
    expect_token(TokenType::RightParen);

    prop.value = quaternion{
        std::stof(std::string(x.lexeme)), std::stof(std::string(y.lexeme)),
        std::stof(std::string(z.lexeme)), std::stof(std::string(w.lexeme))};
  } catch (const std::exception &e) {
    cursor = cursor_backup;
    throw ParseQuaternionException(
        "Quaternion must consist of four numbers separated by commas.");
  }
}

void Parser::parse_property(PropertyNode &prop) {
  Token property = expect_token(TokenType::Identifier);
  prop.name = property.lexeme;
  expect_token(TokenType::Colon);

  Token value = peek_token();
  switch (value.type) {
  case TokenType::Number:
    parse_number(prop, value.lexeme);
    next_token();
    break;
  case TokenType::LeftParen:
    next_token();
    try {
      parse_pair(prop);
    } catch (const ParsePairException &e) {
      try {
        parse_vector3(prop);
      } catch (const ParseVector3Exception &e) {
        try {
          parse_quaternion(prop);
        } catch (const ParseQuaternionException &e) {
          throw ParseException("Invalid property value for " +
                               std::string(prop.name));
        }
      }
    }
    break;
  case TokenType::Identifier:
    prop.value = std::string(value.lexeme);
    next_token();
    break;
  case TokenType::RightBrace: // Block end
    return;
  default:
    throw ParseException("Unexpected token type: " +
                         std::to_string(static_cast<int>(value.type)));
  }
}

void Parser::parse() {
  while (peek_token().type != TokenType::EndOfFile) {
    Token token = expect_token(TokenType::Identifier);

    ObjectNode obj;
    obj.type = token.lexeme;

    expect_token(TokenType::LeftBrace);

    while (peek_token().type != TokenType::RightBrace) {
      PropertyNode prop;
      parse_property(prop);
      obj.properties.push_back(prop);
    }

    expect_token(TokenType::RightBrace); // Consume the RightBrace

    ast.objects.push_back(obj);
  }
}

} // namespace parser

} // namespace tracer