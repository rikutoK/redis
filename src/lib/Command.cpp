#include "Command.hpp"

#include <algorithm>
#include <unordered_map>

static const std::unordered_map<std::string_view, CommandType> map{
    {"PING", CommandType::Ping}, {"GET", CommandType::Get},         {"SET", CommandType::Set},
    {"ECHO", CommandType::Echo}, {"COMMAND", CommandType::Command},
};

CommandType to_command_type(const std::string_view command) {
  std::string c;
  c.reserve(command.size());
  std::ranges::transform(command, std::back_inserter(c), ::toupper);
  auto it = map.find(c);
  if (it != map.end()) {
    return it->second;
  }
  return CommandType::Unknown;
}

namespace std {
template <>
struct hash<CommandType> {
  std::size_t operator()(const CommandType& type) const noexcept {
    return std::hash<int>()(static_cast<int>(type));
  }
};
}  // namespace std

Command::Command(std::vector<std::string> args) {
  if (args.empty()) {
    command_ = CommandType::Unknown;
  } else {
    command_ = to_command_type(args[0]);
    args_.assign(std::make_move_iterator(args.begin() + 1), std::make_move_iterator(args.end()));
  }
}
