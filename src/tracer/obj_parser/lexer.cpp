#include "tracer/obj_parser/lexer.h"

namespace tracer {
namespace obj_parser {

Lexer::Lexer(const std::string &src) : source(src) {
  reserved["v"] = Token(TokenType::VERTEX, "v");
  reserved["vt"] = Token(TokenType::TEXTURE_COORD, "vt");
  reserved["vn"] = Token(TokenType::NORMAL, "vn");
  reserved["f"] = Token(TokenType::FACE, "f");
  reserved["o"] = Token(TokenType::OBJECT, "o");
  reserved["g"] = Token(TokenType::GROUP, "g");
  reserved["usemtl"] = Token(TokenType::USEMTL, "usemtl");
  reserved["mtllib"] = Token(TokenType::MTLLIB, "mtllib");
  reserved["s"] = Token(TokenType::SMOOTH, "s");
  reserved["l"] = Token(TokenType::LINE, "l");
  reserved["p"] = Token(TokenType::POINT, "p");

  reserved["newmtl"] = Token(TokenType::NEWMTL, "newmtl");
  reserved["Ka"] = Token(TokenType::AMBIENT, "Ka");
  reserved["Kd"] = Token(TokenType::DIFFUSE, "Kd");
  reserved["Ks"] = Token(TokenType::SPECULAR, "Ks");
  reserved["Ke"] = Token(TokenType::EMISSION_COLOR, "Ke");
  reserved["Ns"] = Token(TokenType::SHININESS, "Ns");
  reserved["Ni"] = Token(TokenType::INDEX_OF_REFRACTION, "Ni");
  reserved["d"] = Token(TokenType::OPACITY, "d");
  reserved["Tr"] = Token(TokenType::TRANSPARENCY, "Tr");
  reserved["Tf"] = Token(TokenType::TRANSMISSION_FILTER_COLOR, "Tf");
  reserved["illum"] = Token(TokenType::ILLUMINATION_MODEL, "illum");
  reserved["sharpness"] = Token(TokenType::SHARPNESS, "sharpness");

  reserved["map_Kd"] = Token(TokenType::MAP_KD, "map_Kd");
  reserved["map_Ka"] = Token(TokenType::MAP_KA, "map_Ka");
  reserved["map_Ks"] = Token(TokenType::MAP_NS, "map_Ks");
  reserved["map_Ke"] = Token(TokenType::MAP_KE, "map_Ke");
  reserved["map_Ns"] = Token(TokenType::MAP_NS, "map_Ns");
  reserved["map_d"] = Token(TokenType::MAP_D, "map_d");
  reserved["map_Tr"] = Token(TokenType::MAP_TR, "map_Tr");
  reserved["map_bump"] = Token(TokenType::MAP_BUMP, "map_bump");
  reserved["bump"] = Token(TokenType::BUMP, "bump");
  reserved["map_refl"] = Token(TokenType::MAP_REFL, "map_refl");
  reserved["refl"] = Token(TokenType::REFL, "refl");

  reserved["disp"] = Token(TokenType::DISPLACEMENT, "disp");
  reserved["decal"] = Token(TokenType::DECAL, "decal");
}

char Lexer::take() {
  if (cursor < source.length())
    return source[cursor];
  return EOF;
}

char Lexer::peek() {
  if (cursor + 1 < source.length())
    return source[cursor + 1];
  return EOF;
}

void Lexer::consume() {
  if (cursor < source.length())
    ++cursor;
}

void Lexer::skip_whitespaces_and_comments() {
  while (true) {
    char c = take();
    if (std::isspace(c)) {
      consume();
    } else if (c == '#') {
      while (take() != '\n' && take() != EOF) {
        consume();
      }
    } else {
      break;
    }
  }
}

Token Lexer::peek_number() {
  size_t pos = cursor;
  while (isdigit(take()) || take() == '.' || take() == '-') {
    consume();
  }
  return Token(TokenType::NUMBER, source.substr(pos, cursor - pos));
}

Token Lexer::peek_identifier() {
  size_t pos = cursor;
  while (isalnum(take()) || take() == '_' || take() == '.' || take() == '/' ||
         take() == '\\' || take() == '\'' || take() == ':' || take() == '-') {
    consume();
  }
  std::string_view name = source.substr(pos, cursor - pos);
  if (reserved.find(std::string(name)) != reserved.end())
    return reserved[std::string(name)];
  else
    return Token(TokenType::IDENTIFIER, name);
}

std::vector<Token> Lexer::tokenizer() {
  std::vector<Token> token_stream;

  while (cursor < source.length()) {
    skip_whitespaces_and_comments();
    char ch = take();
    Token token;

    if (ch == '/') {
      token = Token(TokenType::SLASH, "/");
      consume();
    } else if (ch == '\n') {
      consume();
      continue;
    } else if (ch == EOF) {
      token = Token(TokenType::ENDOFFILE, "");
      consume();
    } else if (ch == '-' || isdigit(ch)) {
      token = peek_number();
    } else {
      token = peek_identifier();
    }

    token_stream.push_back(token);
  }

  return token_stream;
}

} // namespace obj_parser
} // namespace tracer