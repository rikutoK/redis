#include "RedisObject.hpp"

RedisObject::RedisObject(std::chrono::milliseconds ttl)
    : ttl_(ttl.count() > 0 ? std::optional(std::chrono::steady_clock::now() + ttl) : std::nullopt) {
}

void RedisObject::set_expiry(std::chrono::milliseconds ttl) {
  ttl_ = std::optional(std::chrono::steady_clock::now() + ttl);
}

bool RedisObject::is_expired() const { return ttl_ && *ttl_ < std::chrono::steady_clock::now(); }
