#include "Links/CompoMe/Posix/Udp_server_in/Udp_server_in.hpp"

namespace CompoMe {

namespace Posix {

//============================= addr =============================
CompoMe::String Udp_server_in::get_addr() const { return this->addr; }

void Udp_server_in::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}

CompoMe::String &Udp_server_in::a_addr() { return this->addr; }
//--------------------------------------------------------------------------
//============================= port =============================
i32 Udp_server_in::get_port() const { return this->port; }

void Udp_server_in::set_port(const i32 p_port) { this->port = p_port; }

i32 &Udp_server_in::a_port() { return this->port; }
//--------------------------------------------------------------------------

// Get Port /////////////////////////////////////////////////////////////

CompoMe::Stream::in &Udp_server_in::get_main() { return this->main; }
CompoMe::Stream::map_in &Udp_server_in::get_many() { return this->many; }

} // namespace Posix

} // namespace CompoMe
