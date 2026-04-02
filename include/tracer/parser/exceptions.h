#pragma once
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

} // namespace parser
} // namespace tracer