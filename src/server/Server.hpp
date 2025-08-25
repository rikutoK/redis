#ifndef SERVER_HPP
#define SERVER_HPP

#include <asio.hpp>

#include "CommandExectuor.hpp"
#include "db/Database.hpp"
#include "db/SimpleDatabase.hpp"

class Session;

class Server {
 public:
  Server(short port, std::unique_ptr<Database> db = std::make_unique<SimpleDatabase>());
  void start(int thread_count = std::thread::hardware_concurrency());
  void execute_command(std::shared_ptr<Session> session, Command command);

 private:
  void do_accept();
  asio::io_context io_context_;
  asio::ip::tcp::acceptor acceptor_;
  asio::strand<asio::io_context::executor_type> executor_strand_;
  CommandExecutor executor_;
};

void server_start();

#endif
