#include "Links/CompoMe/Posix/Udp_client_out/Udp_client_out.hpp"
#include "CompoMe/Log.hpp"
#include "Interfaces/Interface.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CompoMe {

namespace Posix {

Udp_client_out::Udp_client_out()
    : CompoMe::Link(), main(), many(), addr("127.0.0.1"), port(8080),
      sockfd(-1) {
  this->main.set_link(*this);
  this->many.set_link(*this);
}

Udp_client_out::~Udp_client_out() { this->main_disconnect(); }

void Udp_client_out::step() { Link::step(); }

void Udp_client_out::main_connect() {
  Link::main_connect();
  C_INFO_TAG("udp,client", "Connection to ", this->get_addr(), ":",
             this->get_port());

  if (sockfd != -1) {
    C_WARNING_TAG("udp,client", "Already Connected");
    return;
  }

  sockaddr_in servaddr = {0};
  servaddr.sin_addr.s_addr = inet_addr(this->get_addr().str.c_str());
  servaddr.sin_port = htons(this->get_port());
  servaddr.sin_family = AF_INET;
  this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->sockfd == -1) {
    C_ERROR_TAG("udp,client", "Socket creation: ", strerror(errno));
  }

  auto r = connect(this->sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
  if (r == -1) {
    C_ERROR_TAG("udp,client", "Connection Error: ", r, strerror(errno));
    return;
  }

  // if (this->a_re == nullptr) {
  //   C_ERROR_TAG("udp,client", "No Interface Provide use set_out()")
  //   return;
  // }

  //  this->f = this->a_re->fake_stream_it(fss, rsr);
  C_INFO_TAG("udp,client", "Connected");
}

void Udp_client_out::main_disconnect() {
  Link::main_disconnect();

  C_INFO_TAG("udp,client", "Disconnection");

  if (this->sockfd != -1) {
    close(this->sockfd);
    this->sockfd = -1;
  } else {
    C_WARNING_TAG("udp,client", "Already Disconnected");
  }

  C_INFO_TAG("udp,client", "Disconnected");
}

// one connect
void Udp_client_out::one_connect(CompoMe::Require_helper &p_r,
                                 CompoMe::String p_key) {
  auto &nc = this->fake_many[p_key];

  nc.fss.set_func_send([this, p_key](CompoMe::String_d &d) {

    if (p_key.str.size() != 0) {
      d.str = p_key.str + "." + d.str;
    }

    auto r = send(this->sockfd, d.str.c_str(), d.str.size(), 0);
    if (r == -1) {
      C_ERROR_TAG("http,client,send", "Send Error : ", strerror(errno));
      this->main_disconnect();
      throw "connection Error";
    }

    return true;
  });

  nc.rss.set_func_recv([this](CompoMe::String_d &d) {
    char l_buffer[1024 + 2];
    auto e = read(this->sockfd, l_buffer, 1024);
    if (e == -1) {
      C_ERROR_TAG("http,client", "Receive error");
      this->main_disconnect();
      return false;
    }

    if (e == 0) {
      C_ERROR_TAG("http,client", "Socket close");
      this->main_disconnect();
      return false;
    }

    l_buffer[e] = ' ';
    l_buffer[e + 1] = '\0';

    std::string str(l_buffer);
    d.str = str;

    return true;
  });

  nc.f = p_r.fake_stream_it(nc.fss, nc.rss);
}

void Udp_client_out::one_connect(CompoMe::Interface &p_i,
                                 CompoMe::String p_key) {
  std::cout << "one_connect interface:" << p_key << "\n";
}

// one disconnect
void Udp_client_out::one_disconnect(CompoMe::Require_helper &p_r,
                                    CompoMe::String p_key) {
  std::cout << "disconnect require:" << p_key << "\n";
}

void Udp_client_out::one_disconnect(CompoMe::Interface &p_i,
                                    CompoMe::String p_key) {
  std::cout << "disconnect interface:" << p_key << "\n";
}

} // namespace Posix

} // namespace CompoMe
