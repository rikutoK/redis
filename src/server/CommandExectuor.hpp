#ifndef COMMANDEXECUTOR_HPP
#define COMMANDEXECUTOR_HPP

#include <memory>

#include "Command.hpp"
#include "db/Database.hpp"

class CommandExecutor {
 public:
  CommandExecutor(std::unique_ptr<Database> db);
  std::string execute(const Command& command);

 private:
  std::string handle_ping(const Command& command);
  std::string handle_echo(const Command& command);
  std::string handle_command(const Command& command);
  std::string handle_get(const Command& command);
  std::string handle_set(const Command& command);
  std::unique_ptr<Database> db_;
};

#endif
