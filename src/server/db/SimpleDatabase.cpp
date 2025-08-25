#include "SimpleDatabase.hpp"

#include <memory>

#include "db/Database.hpp"

RedisObject *SimpleDatabase::get(const std::string &key) {
  if (!exists(key) || map_[key]->is_expired()) return nullptr;
  return map_.at(key).get();
}

RedisObjectPtr SimpleDatabase::set(const std::string &key, RedisObjectPtr value_ptr) {
  RedisObjectPtr prev_value_ptr = del(key);
  map_[key] = std::move(value_ptr);
  if (prev_value_ptr->is_expired()) return RedisObjectPtr{};
  return prev_value_ptr;
}

RedisObjectPtr SimpleDatabase::del(const std::string &key) {
  RedisObjectPtr value_ptr = exists(key) ? std::move(map_[key]) : RedisObjectPtr{};
  map_.erase(key);
  return value_ptr;
}

bool SimpleDatabase::exists(const std::string &key) const { return map_.find(key) != map_.end(); }
