#include "tracer/parser/token.h"

namespace tracer {

namespace parser {

std::string to_string(TokenType type) {
  switch (type) {
  case TokenType::Identifier:
    return "Identifier";
  case TokenType::LeftBrace:
    return "LeftBrace '{'";
  case TokenType::LeftBracket:
    return "LeftBracket '['";
  case TokenType::LeftParen:
    return "LeftParen '('";
  case TokenType::RightBrace:
    return "RightBrace '}'";
  case TokenType::RightBracket:
    return "RightBracket ']'";
  case TokenType::RightParen:
    return "RightParen ')'";
  case TokenType::Colon:
    return "Colon ':'";
  case TokenType::Comma:
    return "Comma ','";
  case TokenType::Equal:
    return "Equal '='";
  case TokenType::Quote:
    return "Quote '\"' or '\''";
  case TokenType::Number:
    return "Number";
  case TokenType::Newline:
    return "Newline '\n'";
  default:
    return "UnknownToken";
  }
}

std::string get_expected_types_string(size_t mask) {
  std::vector<std::string> expected;

  for (int i = 0; i < 64; ++i) {
    size_t bit = 1ULL << i;
    if (mask & bit) {
      expected.push_back(to_string(static_cast<TokenType>(bit)));
    }
  }

  std::stringstream ss;
  for (size_t i = 0; i < expected.size(); ++i) {
    ss << expected[i];
    if (i < expected.size() - 2)
      ss << ", ";
    else if (i == expected.size() - 2)
      ss << " or ";
  }
  return ss.str();
}

} // namespace parser

} // namespace tracer