#include "RespParser.hpp"

#include <cassert>
#include <cctype>

#include "RespSyntax.hpp"

std::vector<Command> RespParser::parse(const char* buffer, int length) {
  this->buffer.append(buffer, length);

  std::vector<Command> commands;
  std::size_t index = 0;
  while (auto command = try_parse_command(index)) {
    commands.push_back(std::move(command.value()));
  }

  this->buffer.erase(this->buffer.begin(), this->buffer.begin() + index);
  return commands;
}

template <typename T, typename ParserFunc>
  requires requires(ParserFunc parser, std::size_t& index) {
    { parser(index) } -> std::convertible_to<std::optional<T>>;
  }
std::optional<std::vector<T>> RespParser::try_parse_array(std::size_t& index, ParserFunc parser) {
  std::size_t start_index = index;
  if (this->buffer[index] != array_prefix) return std::nullopt;
  index++;
  auto int_tok = try_parse_int(index);
  if (!int_tok) {
    index = start_index;
    return std::nullopt;
  }
  int len = int_tok.value();
  if (len == -1) {
    return std::optional<std::vector<T>>{};  // TODO: this should be distinguished from an empty
                                             // array
  }
  std::vector<T> array;
  array.reserve(len);
  for (int i = 0; i < len; i++) {
    std::optional<T> token = parser(index);
    if (!token) {
      index = start_index;
      return std::nullopt;
    }
    array.push_back(token.value());
  }
  return array;
}

std::optional<Command> RespParser::try_parse_command(std::size_t& index) {
  auto args = try_parse_array<std::string_view>(
      index, [this](std::size_t& index) { return try_parse_string(index); });
  if (!args) return std::nullopt;
  std::vector<std::string> args_vec;
  args_vec.reserve(args->size());
  for (const auto& arg : *args) {
    args_vec.emplace_back(arg);
  }
  return std::optional<Command>(std::move(args_vec));
};

std::optional<int> RespParser::try_parse_int(std::size_t& index) {
  if (index >= this->buffer.size()) return std::nullopt;

  std::size_t start_index = index;
  int multiplier = 1;
  if (this->buffer[index] == '-') {
    index++;
    multiplier = -1;
  } else if (this->buffer[index] == '+') {
    index++;
  }

  if (index >= this->buffer.size() || !std::isdigit(this->buffer[index])) {
    index = start_index;
    return std::nullopt;
  }

  int num = 0;
  while (std::isdigit(this->buffer[index])) {
    num *= 10;
    num += this->buffer[index++] - '0';
    if (index >= this->buffer.size()) {
      index = start_index;
      return std::nullopt;
    }
  }
  if (!parse_delimiter(index)) {
    index = start_index;
    return std::nullopt;
  }
  return multiplier * num;
}

bool RespParser::parse_delimiter(std::size_t& index) {
  if (this->buffer.compare(index, delimiter.size(), delimiter) != 0) {
    return false;
  }
  index += delimiter.size();
  return true;
}

std::optional<std::string_view> RespParser::try_parse_string(std::size_t& index) {
  if (index >= this->buffer.size() ||
      (this->buffer[index] != simple_string_prefix && this->buffer[index] != bulk_string_prefix))
    return std::nullopt;
  std::size_t start_index = index;
  int len = 0;
  if (this->buffer[index] == simple_string_prefix) {
    index++;
    std::size_t end_index = this->buffer.find(delimiter, index);
    if (end_index == std::string::npos) {
      index = start_index;
      return std::nullopt;
    }
    len = end_index - index;
  } else {
    index++;
    auto int_tok = try_parse_int(index);
    if (!int_tok) {
      index = start_index;
      return std::nullopt;
    }
    len = int_tok.value();
  }
  assert(index + len <= this->buffer.size());
  std::string_view s(this->buffer.c_str() + index, len);
  index += len;
  if (!parse_delimiter(index)) {
    index = start_index;
    return std::nullopt;
  }
  return s;
}
