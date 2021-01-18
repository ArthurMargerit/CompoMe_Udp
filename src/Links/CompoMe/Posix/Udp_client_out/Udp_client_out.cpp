#include "Links/CompoMe/Posix/Udp_client_out/Udp_client_out.hpp"
#include "CompoMe/Log.hpp"
#include "Interfaces/Interface.hpp"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
namespace cstd {
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
} // namespace cstd

namespace CompoMe {

namespace Posix {

Udp_client_out::Udp_client_out() : CompoMe::Link(), fss(*this), rsr(*this) {}

Udp_client_out::~Udp_client_out() { this->disconnect(); }

void Udp_client_out::step() { Link::step(); }

void Udp_client_out::connect() {
  Link::connect();
  long n;
  cstd::sockaddr_in servaddr = {0};
  servaddr.sin_addr.s_addr = cstd::inet_addr(this->get_addr().str.c_str());
  servaddr.sin_port = cstd::htons(this->get_port());
  servaddr.sin_family = AF_INET;
  this->sockfd = cstd::socket(AF_INET, cstd::SOCK_DGRAM, 0);
  if (this->sockfd == -1) {
    C_ERROR_TAG("udp,client", "Socket creation: ", strerror(errno));
  }

  auto r = cstd::connect(this->sockfd, (cstd::sockaddr *)&servaddr,
                         sizeof(servaddr));
  if (r == -1) {
    C_ERROR_TAG("udp,client", "Connection Error: ", r, strerror(errno));
    return;
  }

  if (this->a_re == nullptr) {
    C_ERROR_TAG("udp,client", "No Interface Provide use set_out()")
    return;
  }

  this->f = this->a_re->fake_stream_it(fss, rsr);
  C_INFO_TAG("udp,client", "Connected");
}

void Udp_client_out::disconnect() {
  Link::disconnect();
  if (this->sockfd != -1) {
    close(sockfd);
    this->sockfd = -1;
    delete this->f;
  }

  C_INFO_TAG("udp,client", "disconnected");
}
// Get and set /////////////////////////////////////////////////////////////

CompoMe::String Udp_client_out::get_addr() const { return this->addr; }

void Udp_client_out::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}
i32 Udp_client_out::get_port() const { return this->port; }

void Udp_client_out::set_port(const i32 p_port) { this->port = p_port; }
CompoMe::String Udp_client_out::get_to_component() const {
  return this->to_component;
}

void Udp_client_out::set_to_component(const CompoMe::String p_to_component) {
  this->to_component = p_to_component;
}
CompoMe::String Udp_client_out::get_to_interface() const {
  return this->to_interface;
}

void Udp_client_out::set_to_interface(const CompoMe::String p_to_interface) {
  this->to_interface = p_to_interface;
}

// stream /////////////////////////////////////////////////////////////

void Function_string_stream_send::send() {
  C_INFO_TAG("udp,client", "ask \"", this->a_ss.str(), "\" to ",
             this->a_l.get_addr(), ":", this->a_l.get_port());
  cstd::sendto(this->a_l.get_sockfd(), this->a_ss.str().c_str(),
               this->a_ss.str().size(), 0, (cstd::sockaddr *)nullptr, 0);
}

void Function_string_stream_send::start() {
  this->a_ss.str("");
  if (this->a_l.get_to_component().str != "") {
    this->a_ss << this->a_l.get_to_component().str << ".";
  }

  if (this->a_l.get_to_interface().str != "") {
    this->a_ss << this->a_l.get_to_interface().str << ".";
  }
}

void Return_string_stream_recv::pull() {
  char buffer[1024 + 2];
  auto r = cstd::recvfrom(this->a_l.get_sockfd(), buffer, 1022, 0,
                          (cstd::sockaddr *)NULL, NULL);
  if (r == -1) {
    C_ERROR("Error: for udp connection", strerror(errno));
    return;
  }
  buffer[r] = ' ';
  buffer[r + 1] = '\0';

  C_INFO_TAG("udp,client", "answer is \"", buffer, "\"");
  std::string str(buffer);
  this->a_ss.str(str);
}

void Return_string_stream_recv::end() { this->a_ss.str(""); }

Function_string_stream_send::Function_string_stream_send(Udp_client_out &l)
    : a_l(l) {}
Return_string_stream_recv::Return_string_stream_recv(Udp_client_out &l)
    : a_l(l) {}

} // namespace Posix

} // namespace CompoMe
