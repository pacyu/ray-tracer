#ifndef TRACER_PARSER_EXCEPTIONS_H
#define TRACER_PARSER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace tracer {
namespace parser {

class ParseException : public std::exception {
public:
  explicit ParseException(const std::string &msg) : msg(msg) {}
  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};

class ParsePairException : public std::exception {
public:
  explicit ParsePairException(const std::string &msg) : msg(msg) {}
  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};

class ParseVector3Exception : public std::exception {
public:
  explicit ParseVector3Exception(const std::string &msg) : msg(msg) {}
  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};

class ParseQuaternionException : public std::exception {
public:
  explicit ParseQuaternionException(const std::string &msg) : msg(msg) {}
  const char *what() const noexcept override { return msg.c_str(); }

private:
  std::string msg;
};

} // namespace parser
} // namespace tracer

#endif // TRACER_PARSER_EXCEPTIONS_H