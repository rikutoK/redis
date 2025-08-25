#ifndef REDISOBJECT_HPP
#define REDISOBJECT_HPP

#include <chrono>
#include <optional>

class RedisObject {
 public:
  virtual ~RedisObject() = default;
  explicit RedisObject() = default;
  [[nodiscard]] virtual std::string to_resp() const = 0;

  void set_expiry(std::chrono::milliseconds ttl);
  [[nodiscard]] bool is_expired() const;

 protected:
  explicit RedisObject(std::chrono::milliseconds ttl);
  std::optional<std::chrono::steady_clock::time_point> ttl_;
};

#endif
