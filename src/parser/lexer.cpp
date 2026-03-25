#include "tracer/parser/lexer.h"

namespace tracer {

namespace parser {

Lexer::Lexer(const std::string &src) : source(src) {
  reserved["Num"] = TokenType::NumberType;
  reserved["String"] = TokenType::String;
  reserved["List"] = TokenType::List;
  reserved["Pair"] = TokenType::Pair;
  reserved["Vec3"] = TokenType::Vector3;
  reserved["Quater"] = TokenType::Quater;
  reserved["Random"] = TokenType::Random;
  reserved["Lambert"] = TokenType::LambertianType;
  reserved["Metal"] = TokenType::MetalType;
  reserved["Dielect"] = TokenType::DielectricType;
  reserved["Light"] = TokenType::LightType;
  reserved["XYRect"] = TokenType::XYRectType;
  reserved["XZRect"] = TokenType::XZRectType;
  reserved["YZRect"] = TokenType::YZRectType;
  reserved["Box"] = TokenType::BoxType;
  reserved["Sphere"] = TokenType::SphereType;
  reserved["Heart"] = TokenType::HeartType;
  reserved["Translate"] = TokenType::TranslateType;
  reserved["ConstMedium"] = TokenType::ConstantMediumType;
  reserved["Camera"] = TokenType::CameraType;
}

char Lexer::peek() const {
  if (cursor >= source.size())
    return EOF;
  return source[cursor];
}

char Lexer::lookahead() const {
  if (cursor + 1 >= source.size())
    return EOF;
  return source[cursor + 1];
}

void Lexer::advance() {
  if (cursor < source.size())
    ++cursor;
}

void Lexer::skip_whitespaces_and_comments() {
  while (true) {
    char c = peek();
    if (std::isspace(c)) {
      advance();
    } else if (c == '#') {
      while (peek() != '\n' && peek() != EOF) {
        advance();
      }
    } else {
      break;
    }
  }
}

Token Lexer::peek_number() {
  size_t start = cursor;
  while (std::isdigit(peek()) || peek() == '.' || peek() == '-') {
    advance();
  }
  return Token(TokenType::Number, source.substr(start, cursor - start));
}

Token Lexer::peek_string() {
  char ch = peek(); // " or '
  advance();
  size_t start = cursor;
  while (peek() != ch) {
    advance();
  }
  advance();
  return Token(TokenType::LiteralStr, source.substr(start, cursor - start - 1));
}

Token Lexer::peek_identifier() {
  size_t start = cursor;
  while (std::isalnum(peek()) || peek() == '_' || peek() == '$') {
    advance();
  }
  std::string_view str = source.substr(start, cursor - start);
  if (reserved.find(std::string(str)) != reserved.end())
    return Token(reserved[std::string(str)], str);
  else
    return Token(TokenType::Identifier, str);
}

Token Lexer::peek_char() {
  size_t start = cursor;
  char c = peek();
  advance();
  switch (c) {
  case '(':
    return Token(TokenType::LeftParen, source.substr(start, 1));
  case ')':
    return Token(TokenType::RightParen, source.substr(start, 1));
  case '[':
    return Token(TokenType::LeftBracket, source.substr(start, 1));
  case ']':
    return Token(TokenType::RightBracket, source.substr(start, 1));
  case '{':
    return Token(TokenType::LeftBrace, source.substr(start, 1));
  case '}':
    return Token(TokenType::RightBrace, source.substr(start, 1));
  case ':':
    return Token(TokenType::Colon, source.substr(start, 1));
  case ',':
    return Token(TokenType::Comma, source.substr(start, 1));
  case '=':
    return Token(TokenType::Equal, source.substr(start, 1));
  case '|':
    return Token(TokenType::Pipe, source.substr(start, 1));
  case '+':
    return Token(TokenType::Plus, source.substr(start, 1));
  case '-':
    return Token(TokenType::Minus, source.substr(start, 1));
  case '*':
    return Token(TokenType::Mul, source.substr(start, 1));
  case '/':
    return Token(TokenType::Div, source.substr(start, 1));
  case '%':
    return Token(TokenType::Mod, source.substr(start, 1));
  case EOF:
    return Token(TokenType::EndOfFile, "");
  default:
    throw std::runtime_error("Unexpected character: " + std::string(1, c));
    break;
  }
}

Token Lexer::peek_ddot() {
  size_t start = cursor;
  advance();
  advance();
  return Token(TokenType::DDot, source.substr(start, cursor - start));
}

Token Lexer::peek_rightarrow() {
  size_t start = cursor;
  advance();
  advance();
  return Token(TokenType::RightArrow, source.substr(start, cursor - start));
}

Token Lexer::next_token() {
  skip_whitespaces_and_comments();
  char c = peek();
  if (c == '.' && lookahead() == '.') {
    return peek_ddot();
  } else if (c == '-' && lookahead() == '>') {
    return peek_rightarrow();
  } else if (std::isdigit(c) || c == '.') {
    return peek_number();
  } else if (c == '"' || c == '\'') {
    return peek_string();
  } else if (std::isalpha(c) || c == '_' || c == '$') {
    return peek_identifier();
  } else {
    return peek_char();
  }
}

std::vector<Token> Lexer::tokenizer() {
  std::vector<Token> tokens;

  while (cursor < source.size()) {
    Token token = next_token();
    if (token.type == TokenType::EndOfFile) {
      tokens.push_back(token);
      break;
    } else if (token.type != TokenType::EndOfFile) {
      tokens.push_back(token);
    } else {
      throw std::runtime_error("Unexpected token: " +
                               std::string(token.lexeme));
    }
  }

  return tokens;
}

} // namespace parser

} // namespace tracer