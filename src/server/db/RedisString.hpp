#ifndef REDISSTRING_HPP
#define REDISSTRING_HPP

#include "RedisObject.hpp"

class RedisString : public RedisObject {
 public:
  RedisString(std::string value, std::chrono::milliseconds ttl = std::chrono::milliseconds(0));
  [[nodiscard]] std::string get_value() const;
  void set_value(std::string value);
  [[nodiscard]] std::string to_resp() const override;

 private:
  std::string value_;
};

#endif
