#ifndef RESPPARSER_HPP
#define RESPPARSER_HPP

#include <optional>
#include <stack>
#include <string>

#include "Command.hpp"

class RespParser {
 public:
  std::vector<Command> parse(const char* buffer, int length);

 private:
  template <typename T, typename ParserFunc>
    requires requires(ParserFunc parser, std::size_t& index) {
      { parser(index) } -> std::convertible_to<std::optional<T>>;
    }
  std::optional<std::vector<T>> try_parse_array(std::size_t& index, ParserFunc parser);
  std::optional<Command> try_parse_command(std::size_t& index);
  std::optional<int> try_parse_int(std::size_t& index);
  bool parse_delimiter(std::size_t& index);
  std::optional<std::string_view> try_parse_string(std::size_t& index);

  std::string buffer;
  std::vector<std::string> args;
  std::stack<int> tokens_left;
};

#endif
