#include "Links/CompoMe/Posix/Udp_server_in/Udp_server_in.hpp"
#include "CompoMe/Log.hpp"
#include "CompoMe/Tools/Call.hpp"
#include "Interfaces/Function_stream_recv.hpp"
#include "Interfaces/Interface.hpp"
#include "Interfaces/Return_stream_send.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024

namespace CompoMe {
namespace Posix {
namespace {
class Return_string_stream_send : public CompoMe::Return_stream_send {
public:
  std::ostream &_os;
  Return_string_stream_send(std::ostream &os) : _os(os) {}
  void start() override {}
  void send() override {}
  std::ostream &get_so() override { return this->_os; }
};

class Function_string_stream_recv : public CompoMe::Function_stream_recv {
public:
  std::istream &_is;
  Function_string_stream_recv(std::istream &is) : _is(is) {}
  void pull() override {}
  void end() override {}
  std::istream &get_si() override { return this->_is; }
};
} // namespace

Udp_server_in::Udp_server_in() : CompoMe::Link() {}

Udp_server_in::~Udp_server_in() {}

void Udp_server_in::step() {
  Link::step();
  char buffer[MAXLINE];

  struct sockaddr_in cliaddr;
  unsigned int len = sizeof(cliaddr);

  auto n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (sockaddr *)&cliaddr,
                    &len);

  if (n == -1) {
    return;
  }

  buffer[n] = '\0';
  C_INFO_TAG("udp,server,call", "Call: ", buffer, " from ");
  auto result = CompoMe::Tools::call(&this->get_caller_stream(), buffer);
  C_INFO_TAG("udp,server,call", "Respond : ", result.second);
  sendto(sockfd, result.second.c_str(), result.second.size(), 0,
         (sockaddr *)&cliaddr, len);
}

void Udp_server_in::connect() {
  Link::connect();

  // Creating socket file descriptor
  this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->sockfd < 0) {
    C_ERROR_TAG("udp,server", "Socket creation ", strerror(errno));
    return;
  }

  struct sockaddr_in servaddr = {0};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(this->get_port());
  servaddr.sin_addr.s_addr = inet_addr(this->get_addr().str.c_str());

  auto r =
      bind(this->sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
  if (r < 0) {
    C_ERROR_TAG("Bind failed ", strerror(errno));
  }

  long save_file_flags = fcntl(sockfd, F_GETFL);
  save_file_flags |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, SOCK_NONBLOCK);
  C_INFO_TAG("udp,server", "Udp Server started at ", this->get_addr(), ":",
             this->get_port());
}

void Udp_server_in::disconnect() {
  Link::disconnect();
  close(this->sockfd);

  C_INFO_TAG("udp,server", "Udp Server disconnected form", this->get_addr(),
             ":", this->get_port());
}

// Get and set /////////////////////////////////////////////////////////////

CompoMe::String Udp_server_in::get_addr() const { return this->addr; }

void Udp_server_in::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}
i32 Udp_server_in::get_port() const { return this->port; }

void Udp_server_in::set_port(const i32 p_port) { this->port = p_port; }

} // namespace Posix

} // namespace CompoMe
