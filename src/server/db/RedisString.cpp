#include "RedisString.hpp"

#include "RespSyntax.hpp"

RedisString::RedisString(std::string value, std::chrono::milliseconds ttl)
    : RedisObject(ttl), value_(std::move(value)) {}

std::string RedisString::get_value() const { return value_; }

void RedisString::set_value(std::string value) { value_ = std::move(value); }

std::string RedisString::to_resp() const {
  std::string resp;
  std::string length_str = std::to_string(value_.size());
  resp.reserve(1 + length_str.size() + value_.size() + 2 * delimiter.size());
  resp += std::string(1, bulk_string_prefix);
  resp += length_str;
  resp += delimiter;
  resp += value_;
  resp += delimiter;
  return resp;
}