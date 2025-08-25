#include <gtest/gtest.h>

#include "lib/RespParser.hpp"

class RespParserTest : public ::testing::Test {
 protected:
  void SetUp() override { parser = RespParser(); }

  void TearDown() override {}

  RespParser parser;
};

TEST_F(RespParserTest, CanParseSimpleString) {
  std::string input = "*1\r\n+PING\r\n";
  auto commands = parser.parse(input.c_str(), input.size());
  ASSERT_EQ(commands.size(), 1);
  ASSERT_EQ(commands[0], Command({"PING"}));
}

TEST_F(RespParserTest, CanParseBulkString) {
  std::string input = "*1\r\n$4\r\nPING\r\n";
  auto commands = parser.parse(input.c_str(), input.size());
  ASSERT_EQ(commands.size(), 1);
  ASSERT_EQ(commands[0], Command({"PING"}));
}

TEST_F(RespParserTest, CanHandlePartialInputsIncrementally) {
  std::string input1 = "*1\r\n+PI";
  std::string input2 = "NG\r\n";
  auto commands1 = parser.parse(input1.c_str(), input1.size());
  ASSERT_EQ(commands1.size(), 0);
  auto commands2 = parser.parse(input2.c_str(), input2.size());
  ASSERT_EQ(commands2.size(), 1);
  ASSERT_EQ(commands2[0], Command({"PING"}));
}

TEST_F(RespParserTest, CanHandleBatchedCommands) {
  std::string input = "*1\r\n+PING\r\n*1\r\n+ECHO\r\n";
  auto commands = parser.parse(input.c_str(), input.size());
  ASSERT_EQ(commands.size(), 2);
  ASSERT_EQ(commands[0], Command({"PING"}));
  ASSERT_EQ(commands[1], Command({"ECHO"}));
}

TEST_F(RespParserTest, CanHandleMultipleCommands) {
  std::string input1 = "*1\r\n+PING\r\n";
  std::string input2 = "*1\r\n+ECHO\r\n";
  auto commands1 = parser.parse(input1.c_str(), input1.size());
  ASSERT_EQ(commands1.size(), 1);
  ASSERT_EQ(commands1[0], Command({"PING"}));
  auto commands2 = parser.parse(input2.c_str(), input2.size());
  ASSERT_EQ(commands2.size(), 1);
  ASSERT_EQ(commands2[0], Command({"ECHO"}));
}
