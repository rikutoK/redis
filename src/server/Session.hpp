#ifndef SESSION_HPP
#define SESSION_HPP

#include <asio.hpp>
#include <deque>
#include <mutex>

#include "RespParser.hpp"

class Server;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(asio::ip::tcp::socket socket, Server& server);
  void start();
  void queue_write(std::string response);

 private:
  void do_read();
  void do_write(std::string response);
  static constexpr int max_length = 1024;
  asio::ip::tcp::socket socket_;
  char data_[max_length];
  RespParser parser_{};
  std::deque<std::string> write_queue_;
  std::mutex queue_mutex_;
  Server& server_;
};

#endif
