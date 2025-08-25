#include <functional>
#include <iostream>

#include "CommandExectuor.hpp"
#include "RespSyntax.hpp"
#include "db/RedisString.hpp"

#define CHECK_ARGC_PRED(command_name, command, predicate)                       \
  if (!predicate(command.args_.size())) {                                       \
    return "-ERR wrong number of arguments for '" command_name "' command\r\n"; \
  }

CommandExecutor::CommandExecutor(std::unique_ptr<Database> db) : db_(std::move(db)) {}

std::string CommandExecutor::execute(const Command& command) {
  std::cout << "Command type* : " << static_cast<int>(command.command_) << "\n";
  switch (command.command_) {
    case CommandType::Ping:
      return handle_ping(command);
    case CommandType::Echo:
      return handle_echo(command);
    case CommandType::Command:
      return handle_command(command);
    case CommandType::Get:
      return handle_get(command);
    case CommandType::Set:
      return handle_set(command);
    default:
      return "-ERR unknown command\r\n";
  }
}

std::string CommandExecutor::handle_ping(const Command& command) {
  CHECK_ARGC_PRED("PING", command, [](size_t argc) { return argc == 0; });
  return "+PONG\r\n";
}

std::string CommandExecutor::handle_echo(const Command& command) {
  CHECK_ARGC_PRED("ECHO", command, [](size_t argc) { return argc == 1; });

  const auto& arg = command.args_[0];
  std::string response;
  response.reserve(2 + arg.size() + 2 * delimiter.size());
  response += std::string(1, bulk_string_prefix);
  response += std::to_string(arg.size());
  response += delimiter;
  response += arg;
  response += delimiter;
  return response;
}

std::string CommandExecutor::handle_command(const Command& command) {
  return "*0\r\n";
  // "*1\r\n"
  // "*2\r\n"
  // "$4\r\nPING\r\n"
  // "*2\r\n"
  // "$7\r\nsummary\r\n"
  // "$15\r\nPing the server\r\n";
}

std::string CommandExecutor::handle_get(const Command& command) {
  CHECK_ARGC_PRED("GET", command, [](size_t argc) { return argc == 1; });

  const auto& key = command.args_[0];
  RedisObject* value = db_->get(key);
  if (!value) return null_bulk_string;
  return value->to_resp();
}

std::string CommandExecutor::handle_set(const Command& command) {
  CHECK_ARGC_PRED("SET", command, [](size_t argc) { return argc == 2; });

  const auto& key = command.args_[0];
  const auto& value = command.args_[1];
  db_->set(key, std::make_unique<RedisString>(value));

  return "+OK\r\n";
}