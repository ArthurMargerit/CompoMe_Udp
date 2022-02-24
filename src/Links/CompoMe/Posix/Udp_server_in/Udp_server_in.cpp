#include "Links/CompoMe/Posix/Udp_server_in/Udp_server_in.hpp"
#include "CompoMe/Log.hpp"
#include "CompoMe/Tools/Call.hpp"
#include "Interfaces/Function_stream_recv.hpp"
#include "Interfaces/Interface.hpp"
#include "Interfaces/Return_stream_send.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define MAXLINE 1024

namespace CompoMe {

namespace Posix {

Udp_server_in::Udp_server_in()
  : CompoMe::Link(), main(), many(), addr("127.0.0.1"), port(8080) {
  this->main.set_link(*this);
  this->many.set_link(*this);
}

Udp_server_in::~Udp_server_in() {}

void Udp_server_in::step() {
  Link::step();

  char buffer[MAXLINE];

  struct sockaddr_in cliaddr;
  unsigned int len = sizeof(cliaddr);

  struct pollfd l_poll_fd;

  l_poll_fd.fd = this->sockfd;
  l_poll_fd.events = POLLIN | POLLERR | POLLHUP;
  l_poll_fd.revents = 0;

  int ret = poll(&l_poll_fd, 1, this->get_poll_time());
  if (ret == 0) {
    return;
  }

  while(true) {
    auto n = recvfrom(this->sockfd, buffer, MAXLINE, MSG_WAITALL,
                      (sockaddr *)&cliaddr, &len);

    if (n == -1) {
      break;
    }

    buffer[n] = '\0';
    C_INFO_TAG("udp,server,call", "Call: ", buffer, " from ");

    auto result = (buffer[0] == '/') ? this->get_many().call(buffer)
      : this->get_main().call(buffer);

    C_INFO_TAG("udp,server,call", "Respond : ", result.second);

    sendto(this->sockfd, (result.second.str.size()) ? result.second.str.c_str() : " ",
           (result.second.str.size()) ? result.second.str.size() : 1, 0,
           (sockaddr *)&cliaddr, len);
  }
}

void Udp_server_in::main_connect() {
  Link::main_connect();

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
  fcntl(this->sockfd, F_SETFL, SOCK_NONBLOCK);
  C_INFO_TAG("udp,server", "Udp Server started at ", this->get_addr(), ":",
             this->get_port());
}

void Udp_server_in::main_disconnect() {
  Link::main_disconnect();
  close(this->sockfd);

  C_INFO_TAG("udp,server", "Udp Server disconnected form", this->get_addr(),
             ":", this->get_port());
}

// one connect
void Udp_server_in::one_connect(CompoMe::Require_helper &p_r,
                                CompoMe::String p_key) {}

void Udp_server_in::one_connect(CompoMe::Interface &p_i,
                                CompoMe::String p_key) {}

// one disconnect
void Udp_server_in::one_disconnect(CompoMe::Require_helper &p_r,
                                   CompoMe::String p_key) {}

void Udp_server_in::one_disconnect(CompoMe::Interface &p_i,
                                   CompoMe::String p_key) {}

} // namespace Posix

} // namespace CompoMe
