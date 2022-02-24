#pragma once

#include "Data/CompoMe_Udp.hpp"

#include "Links/Link.hpp"

// TYPES

#include "Types/CompoMe/String.hpp"

#include "Types/i32.hpp"

#include "Types/ui32.hpp"
// STRUCT

// PORT

#include "Ports/CompoMe/Stream/in.hpp"

#include "Ports/CompoMe/Stream/map_in.hpp"

namespace CompoMe {

namespace Posix {

class Udp_server_in : public CompoMe::Link {
public:
  Udp_server_in();
  virtual ~Udp_server_in();

  void step() override;
  void main_connect() override;
  void main_disconnect() override;

  // one connect
  void one_connect(CompoMe::Require_helper &, CompoMe::String c) override;
  void one_connect(CompoMe::Interface &, CompoMe::String) override;

  // one disconnect
  void one_disconnect(CompoMe::Require_helper &, CompoMe::String) override;
  void one_disconnect(CompoMe::Interface &, CompoMe::String) override;

  // Get and set /////////////////////////////////////////////////////////////

  CompoMe::String get_addr() const;
  void set_addr(const CompoMe::String addr);
  CompoMe::String &a_addr();
  i32 get_port() const;
  void set_port(const i32 port);
  i32 &a_port();
  ui32 get_poll_time() const;
  void set_poll_time(const ui32 poll_time);
  ui32 &a_poll_time();

  // Get Port /////////////////////////////////////////////////////////////

  CompoMe::Stream::in &get_main();
  CompoMe::Stream::map_in &get_many();

public:
  // Function
  // ///////////////////////////////////////////////////////////////////

private:
  // DATA ////////////////////////////////////////////////////////////////////
  CompoMe::String addr;
  i32 port;
  ui32 poll_time;
  i32 sockfd;

  // PORT ////////////////////////////////////////////////////////////////////
  CompoMe::Stream::in main;
  CompoMe::Stream::map_in many;
};

} // namespace Posix

} // namespace CompoMe
