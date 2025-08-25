#include <gtest/gtest.h>

#include "lib/Command.hpp"

class CommandTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(CommandTest, CanConstructCommands) {
  Command command({"PING", "arg1", "arg2"});
  ASSERT_EQ(command.command_, CommandType::Ping);
  ASSERT_EQ(command.args_.size(), 2);
}

TEST_F(CommandTest, CanConstructUnknownCommand) {
  Command command({"NonExistentCommand"});
  ASSERT_EQ(command.command_, CommandType::Unknown);
  ASSERT_EQ(command.args_.size(), 0);
}
