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
//============================= poll_time =============================
ui32 Udp_server_in::get_poll_time() const { return this->poll_time; }

void Udp_server_in::set_poll_time(const ui32 p_poll_time) {
  this->poll_time = p_poll_time;
}

ui32 &Udp_server_in::a_poll_time() { return this->poll_time; }
//--------------------------------------------------------------------------

void Udp_server_in::set_size_max_message(const ui32 size_max_message) {
  this->size_max_message = p_size_max_message;
}

ui32 Udp_server_in::get_size_max_message() { return p_size_max_message; }

// Get Port /////////////////////////////////////////////////////////////

CompoMe::Stream::in &Udp_server_in::get_main() { return this->main; }
CompoMe::Stream::map_in &Udp_server_in::get_many() { return this->many; }

} // namespace Posix

} // namespace CompoMe
