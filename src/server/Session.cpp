#include "Session.hpp"

#include <iostream>

#include "Server.hpp"

using asio::ip::tcp;

Session::Session(tcp::socket socket, Server &server)
    : socket_(std::move(socket)), server_(server) {}

void Session::start() {
  std::cout << "Client connected\n";
  do_read();
}

void Session::queue_write(std::string response) {
  bool write_in_progress;
  std::string front;
  {
    std::lock_guard lock(queue_mutex_);
    write_in_progress = !write_queue_.empty();
    write_queue_.push_back(std::move(response));
    front = std::move(write_queue_.front());
  }
  if (!write_in_progress) {
    do_write(std::move(front));
  }
}

void Session::do_read() {
  std::shared_ptr<Session> self(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, max_length),
                          [this, self](std::error_code code, std::size_t length) {
                            if (code == asio::error::eof) {
                              std::cout << "Client disconnected\n";
                              socket_.close();
                              return;
                            }
                            if (code) {
                              std::cerr << "Error in read: " << code.message() << "\n";
                              socket_.close(code);
                              return;
                            }

                            std::string_view request(data_, length);
                            std::cout << "Message recieved from client: " << request << "\n";

                            std::vector<Command> commands = parser_.parse(data_, length);
                            for (Command &command : commands) {
                              server_.execute_command(self, std::move(command));
                            }

                            // Register another read
                            do_read();
                          });
}

void Session::do_write(std::string response) {
  std::cout << "Response to client: " << response << "\n";

  std::shared_ptr<Session> self(shared_from_this());
  std::unique_ptr<std::string> response_ptr = std::make_unique<std::string>(std::move(response));
  asio::async_write(
      socket_, asio::buffer(*response_ptr),
      [this, self, response_ptr = std::move(response_ptr)](std::error_code code, std::size_t) {
        if (code) {
          std::cerr << "Error on write: " << code.message() << "\n";
          socket_.close(code);
          return;
        }

        // Check if there are more responses to send
        bool has_more;
        std::string next_response;
        {
          std::lock_guard lock(queue_mutex_);
          write_queue_.pop_front();
          has_more = !write_queue_.empty();
          if (has_more) {
            next_response = std::move(write_queue_.front());
          }
        }
        if (has_more) {
          do_write(std::move(next_response));
        }
      });
}
