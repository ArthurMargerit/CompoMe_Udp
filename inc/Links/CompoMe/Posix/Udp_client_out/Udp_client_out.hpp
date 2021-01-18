#pragma once

#include "Links/Link.hpp"

namespace CompoMe {
class Function_stream;
class Return_stream;
class Interface;
} // namespace CompoMe

#include "Data/CompoMe_Udp.hpp"

namespace CompoMe {

namespace Posix {
class Udp_client_out;

class Function_string_stream_send : public CompoMe::Function_stream_send {
private:
  std::stringstream a_ss;
  Udp_client_out &a_l;

public:
  Function_string_stream_send(Udp_client_out &l);
  void start() override;
  void send() override;
  std::ostream &get_so() override { return this->a_ss; }
};

class Return_string_stream_recv : public CompoMe::Return_stream_recv {
private:
  std::stringstream a_ss;
  Udp_client_out &a_l;

public:
  Return_string_stream_recv(Udp_client_out &l);
  void pull() override;
  void end() override;
  std::istream &get_si() override { return this->a_ss; }
};

class Udp_client_out : public CompoMe::Link, public CompoMe::Link_out {
public:
  Udp_client_out();
  virtual ~Udp_client_out();

  void step() override;
  void connect() override;
  void disconnect() override;

  // Get and set /////////////////////////////////////////////////////////////

  virtual CompoMe::String get_addr() const;
  virtual void set_addr(const CompoMe::String addr);
  virtual i32 get_port() const;
  virtual void set_port(const i32 port);
  virtual CompoMe::String get_to_component() const;
  virtual void set_to_component(const CompoMe::String to_component);
  virtual CompoMe::String get_to_interface() const;
  virtual void set_to_interface(const CompoMe::String to_interface);

public:
  int get_sockfd() const { return this->sockfd; }

  // Function
  // ///////////////////////////////////////////////////////////////////
private:
  // DATA ////////////////////////////////////////////////////////////////////

  CompoMe::String addr;

  i32 port;
  int sockfd;
  CompoMe::Fake_stream *f;
  Function_string_stream_send fss;
  Return_string_stream_recv rsr;

  CompoMe::String to_component;

  CompoMe::String to_interface;
};

} // namespace Posix

} // namespace CompoMe
