#ifndef SIMPLEDATABASE_HPP
#define SIMPLEDATABASE_HPP

#include <unordered_map>

#include "Database.hpp"

class SimpleDatabase : public Database {
 public:
  RedisObject* get(const std::string& key) override;
  RedisObjectPtr set(const std::string& key, RedisObjectPtr value_ptr) override;
  RedisObjectPtr del(const std::string& key) override;
  [[nodiscard]] bool exists(const std::string& key) const override;

 private:
  std::unordered_map<std::string, RedisObjectPtr> map_;
};

#endif
