#include "Links/CompoMe/Posix/Udp_server_in/Udp_server_in.hpp"
#include "CompoMe/Log.hpp"
#include "CompoMe/Tools/Call.hpp"
#include "Interfaces/Function_stream_recv.hpp"
#include "Interfaces/Interface.hpp"
#include "Interfaces/Return_stream_send.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>

#define DEFAULT_ADDR ("127.0.0.1")
#define DEFAULT_PORT (8080)
#define DEFAULT_MAX_SIZE (4096)
#define DEFAULT_POLL_TIME (10)

namespace CompoMe {

namespace Posix {

Udp_server_in::Udp_server_in()
    : CompoMe::Link(), main(), many(), addr(DEFAULT_ADDR), port(DEFAULT_PORT),
      poll_time(DEFAULT_POLL_TIME), buff(nullptr),
      size_max_message(DEFAULT_MAX_SIZE) {
  this->main.set_link(*this);
  this->many.set_link(*this);
}

Udp_server_in::~Udp_server_in() {}

void Udp_server_in::step() {
  Link::step();
 
  struct sockaddr_in cliaddr;
  unsigned int len = sizeof(cliaddr);
 
  struct pollfd l_poll_fd;
 
  l_poll_fd.fd = this->sockfd;
  l_poll_fd.events = POLLIN | POLLERR | POLLHUP;
  l_poll_fd.revents = 0;
 
  int ret = poll(&l_poll_fd, 1, this->get_poll_time());
 
  if (ret == 0) {
    C_ERROR_TAG("poll 0");
    return;
  }
 
  while (true) {
    auto n = recvfrom(this->sockfd, buff, this->get_size_max_message(),
                      MSG_WAITALL, (sockaddr *)&cliaddr, &len);
 
    if (n == -1) {
      break;
    }
 
    buff[n] = '\0';
    C_INFO_TAG("udp,server,call: Call:", buff, " from ");
 
    auto result = (buff[0] == '/') ? this->get_many().call(buff)
                                   : this->get_main().call(buff);
 
    C_INFO_TAG("udp,server,call: Respond : ", result.second);
 
    
    size_t max_message_size = this->get_size_max_message();
    std::string &response = result.second.str;
    size_t total_size = response.size();
    size_t sent_size = 0;
    char l_save_char_under_limiter = 0;
 
    // If we have nothing to send, we send a space to avoid the client to wait for a timeout
    if (total_size == 0)
    {
      C_INFO_TAG("udp,server,call: Empty response, nothing to send.");
      sendto(this->sockfd,
                " ",
                1, 0,
                (sockaddr *)&cliaddr, len);
      break;
    }
 
    do {
      size_t chunk_size = std::min(max_message_size, (total_size - sent_size));
 
      // If the message is chunked, we had a # and we save the last character that is not sent
      if(chunk_size == max_message_size && (total_size - sent_size) > max_message_size)
      {
        l_save_char_under_limiter = response.at(chunk_size - 1);
        response.at(sent_size + chunk_size - 1) = '#';
      }
 
      sendto(this->sockfd, (response.c_str() + sent_size), chunk_size, 0,
              (sockaddr *)&cliaddr, len);
 
      // If the message is not complete, set the saved character again in the buffer and update the sent size
      if (chunk_size == max_message_size && (total_size - sent_size) > max_message_size) {
        response.at(sent_size + chunk_size - 1) = l_save_char_under_limiter;
        sent_size += (chunk_size - 1); 
      }
      else {
        sent_size += chunk_size;
      }

    } while (sent_size < total_size);
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

  if (this->buff != nullptr) {
    free(this->buff);
  }

  this->buff = (char *)malloc(sizeof(char) * this->size_max_message + 1);
}

void Udp_server_in::main_disconnect() {
  Link::main_disconnect();
  close(this->sockfd);

  C_INFO_TAG("udp,server", "Udp Server disconnected form", this->get_addr(),
             ":", this->get_port());
  if (this->buff != nullptr) {
    free(this->buff);
  }
  this->buff = nullptr;
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
