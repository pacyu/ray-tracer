#include "tracer/parser/lexer.h"

namespace tracer {

namespace parser {

char Lexer::peek() const {
  if (cursor >= source.size())
    return EOF;
  return source[cursor];
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

Token Lexer::peek_identifier() {
  size_t start = cursor;
  while (std::isalnum(peek()) || peek() == '_' || peek() == '-') {
    advance();
  }
  return Token(TokenType::Identifier, source.substr(start, cursor - start));
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
  case '{':
    return Token(TokenType::LeftBrace, source.substr(start, 1));
  case '}':
    return Token(TokenType::RightBrace, source.substr(start, 1));
  case ':':
    return Token(TokenType::Colon, source.substr(start, 1));
  case ',':
    return Token(TokenType::Comma, source.substr(start, 1));
  case EOF:
    return Token(TokenType::EndOfFile, "");
  default:
    throw std::runtime_error("Unexpected character: " + std::string(1, c));
    break;
  }
}

Token Lexer::next_token() {
  skip_whitespaces_and_comments();
  char c = peek();
  if (std::isdigit(c) || c == '.' || c == '-') {
    return peek_number();
  } else if (std::isalpha(c) || c == '_') {
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