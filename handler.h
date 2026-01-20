#ifndef DREADMYST_HANDLER_H
#define DREADMYST_HANDLER_H

#include "common.h"
#include <string>

namespace network {

	class handler {
	public:
		static bool connect_to_server();
		static bool send_auth_packet(const std::string& token);
		static bool send_character_select(DWORD character_id);
		
		static std::string generate_random_fingerprint();

	private:
		static packet_buffer* create_packet_buffer(void* data, size_t size);
		static void send_packet(packet_buffer* packet);
	};
} 

#endif
