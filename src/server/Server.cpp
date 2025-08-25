#include "Server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "Session.hpp"

using asio::ip::tcp;

Server::Server(short port, std::unique_ptr<Database> db)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
      executor_strand_(io_context_.get_executor()),
      executor_(std::move(db)) {
  do_accept();
}

void Server::start(int thread_count) {
  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back([this]() { io_context_.run(); });
  }
  for (auto &t : threads) {
    t.join();
  }
}

void Server::do_accept() {
  acceptor_.async_accept([this](std::error_code code, tcp::socket socket) {
    if (!code) {
      std::make_shared<Session>(std::move(socket), *this)->start();
    }
    do_accept();
  });
}

void Server::execute_command(std::shared_ptr<Session> session, Command command) {
  asio::post(executor_strand_,
             [this, session = std::move(session), command = std::move(command)]() {
               std::string response = executor_.execute(command);
               session->queue_write(std::move(response));
             });
}

void server_start() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  try {
    Server server(6379);
    std::cout << "Server running on port 6379\n";
    server.start();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  /* select approach */
  // int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // if (server_fd < 0) {
  //  std::cerr << "Failed to create server socket\n";
  //  return 1;
  // }

  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  // int reuse = 1;
  // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))
  // < 0) {
  //   std::cerr << "setsockopt failed\n";
  //   return 1;
  // }

  // struct sockaddr_in server_addr;
  // server_addr.sin_family = AF_INET;
  // server_addr.sin_addr.s_addr = INADDR_ANY;
  // server_addr.sin_port = htons(6379);

  // if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))
  // != 0) {
  //   std::cerr << "Failed to bind to port 6379\n";
  //   return 1;
  // }

  // int connection_backlog = 5;
  // if (listen(server_fd, connection_backlog) != 0) {
  //   std::cerr << "listen failed\n";
  //   return 1;
  // }

  // char buffer[1024];

  // fd_set master_set, read_set;
  // FD_ZERO(&master_set);
  // FD_SET(server_fd, &master_set);
  // int max_fd = server_fd;

  // while (true) {
  //   read_set = master_set;
  //   int ready_count = select(max_fd + 1, &read_set, nullptr, nullptr,
  //   nullptr);

  //   if (ready_count < 0) {
  //     std::cerr << "select failed\n";
  //     continue;
  //   }

  //   for (int fd = 0; fd <= max_fd; fd++) {
  //     if (!FD_ISSET(fd, &read_set)) continue;

  //     if (fd == server_fd) { // New client connection
  //       struct sockaddr_in client_addr;
  //       int client_addr_len = sizeof(client_addr);
  //       int client_fd = accept(server_fd, (struct sockaddr *) &client_addr,
  //       (socklen_t *) &client_addr_len); if (client_fd < 0) {
  //         std::cerr << "accept failed\n";
  //         continue;
  //       }
  //       std::cout << "Connected new client\n";
  //       FD_SET(client_fd, &master_set);
  //       max_fd = std::max(max_fd, client_fd);
  //     } else { // Exisitng client request
  //       ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
  //       if (bytes_read <= 0) {
  //         std::cout << "Client disconnected: FD " << fd << "\n";
  //         close(fd);
  //         FD_CLR(fd, &master_set);
  //       } else {
  //         std::string_view request(buffer, bytes_read);
  //         if (request.find("PING") != std::string::npos) {
  //           std::string response = "+PONG\r\n";
  //           send(fd, response.c_str(), response.size(), 0);
  //         }
  //       }
  //     }
  //   }
  // }

  // close(server_fd);
}
