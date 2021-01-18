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

class Udp_server_in : public CompoMe::Link, public CompoMe::Link_in {
public:
  Udp_server_in();
  virtual ~Udp_server_in();

  void step() override;
  void connect() override;
  void disconnect() override;

  // Get and set /////////////////////////////////////////////////////////////

  virtual CompoMe::String get_addr() const;
  virtual void set_addr(const CompoMe::String addr);
  virtual i32 get_port() const;
  virtual void set_port(const i32 port);

private:
  // DATA ////////////////////////////////////////////////////////////////////

  CompoMe::String addr;

  i32 port;

  int sockfd;
};

} // namespace Posix

} // namespace CompoMe
