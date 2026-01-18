#ifndef DREADMYST_NETWORK_H
#define DREADMYST_NETWORK_H

#include "common.h"
#include <string>


namespace network {

	class handler {
	public:
		static bool connect_to_server();
		static bool send_auth_packet(const std::string& token);
		static bool send_character_select(DWORD character_id);
		static bool send_move_packet(int x, int y);
		static bool send_attack_packet(DWORD target_id);

	private:
		static packet_buffer* create_packet_buffer(const std::vector<unsigned char>& data);
		static void send_packet(packet_buffer* packet);
	};

} 

#endif 