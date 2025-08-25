#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

#include "RedisObject.hpp"

using RedisObjectPtr = std::unique_ptr<RedisObject>;

class Database {
 public:
  virtual ~Database() = default;

  virtual RedisObject* get(const std::string& key) = 0;
  virtual RedisObjectPtr set(const std::string& key, RedisObjectPtr value_ptr) = 0;
  virtual RedisObjectPtr del(const std::string& key) = 0;
  [[nodiscard]] virtual bool exists(const std::string& key) const = 0;
};

#endif
