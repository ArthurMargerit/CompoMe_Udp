#include "Links/CompoMe/Posix/Udp_client_out/Udp_client_out.hpp"

namespace CompoMe {

namespace Posix {

//============================= addr =============================
CompoMe::String Udp_client_out::get_addr() const { return this->addr; }

void Udp_client_out::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}

CompoMe::String &Udp_client_out::a_addr() { return this->addr; }
//--------------------------------------------------------------------------
//============================= port =============================
i32 Udp_client_out::get_port() const { return this->port; }

void Udp_client_out::set_port(const i32 p_port) { this->port = p_port; }

i32 &Udp_client_out::a_port() { return this->port; }
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void Udp_client_out::set_size_max_message(const ui32 p_size_max_message) {
  this->size_max_message = p_size_max_message;
}

ui32 Udp_client_out::get_size_max_message() { return this->size_max_message; }

// Get Port /////////////////////////////////////////////////////////////

CompoMe::Stream::out &Udp_client_out::get_main() { return this->main; }
CompoMe::Stream::map_out &Udp_client_out::get_many() { return this->many; }

} // namespace Posix

} // namespace CompoMe
