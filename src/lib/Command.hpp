#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

enum class CommandType {
  Ping,
  Echo,
  Get,
  Set,
  Command,
  Unknown,
};

CommandType to_command_type(std::string_view command);

struct Command {
  CommandType command_;
  std::vector<std::string> args_;

  Command(std::vector<std::string> args);
  auto operator<=>(const Command& other) const = default;
};

#endif
