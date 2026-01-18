#include "handler.h"
#include "manager.h"
#include <stdio.h>

namespace network {

	packet_buffer* handler::create_packet_buffer(const std::vector<unsigned char>& data) {
		packet_buffer* packet = (packet_buffer*)malloc(sizeof(packet_buffer));
		memset(packet, 0, sizeof(packet_buffer));

		char* buffer = (char*)malloc(data.size());
		memcpy(buffer, data.data(), data.size());

		packet->m_data_start = buffer;
		packet->m_data_end = buffer + data.size();

		return packet;
	}

	void handler::send_packet(packet_buffer* packet) {
		void* network_mgr = game::manager::get_network_manager();
		if (!network_mgr) {
			free(packet->m_data_start);
			free(packet);
			return;
		}

		pfn_game_send_packet send_func = game::manager::get_send_packet_func();
		if (send_func) {
			send_func(network_mgr, packet);
		}

		free(packet->m_data_start);
		free(packet);
	}

	bool handler::connect_to_server() {
		void* network_mgr = game::manager::get_network_manager();
		if (!network_mgr) {
			return false;
		}

		DWORD base_addr = game::manager::get_base_address();
		DWORD connect_func = base_addr + 0x24DB0;
		char result = 0;

		__asm {
			push ebp
			mov ecx, network_mgr
			mov eax, connect_func
			call eax
			mov result, al
			pop ebp
		}

		return result != 0;
	}

	bool handler::send_auth_packet(const std::string& token) {
		if (token.empty()) {
			return false;
		}


		std::vector<unsigned char> data;

		
		data.push_back(0x02);
		data.push_back(0x00);

		
		for (char c : token) {
			data.push_back((unsigned char)c);
		}
		data.push_back(0x00);

		
		data.push_back(0xAD);
		data.push_back(0x04);
		data.push_back(0x00);
		data.push_back(0x00);

		
		std::string hardware_id = "91bb435ce51beb36cf62eaad847b024d";
		for (char c : hardware_id) {
			data.push_back((unsigned char)c);
		}
		data.push_back(0x00);

		packet_buffer* packet = create_packet_buffer(data);
		send_packet(packet);


		return true;
	}

	bool handler::send_character_select(DWORD character_id) {
		

		std::vector<unsigned char> data;

		
		data.push_back(0x05);
		data.push_back(0x00);

	
		data.push_back((character_id) & 0xFF);
		data.push_back((character_id >> 8) & 0xFF);
		data.push_back((character_id >> 16) & 0xFF);
		data.push_back((character_id >> 24) & 0xFF);

		packet_buffer* packet = create_packet_buffer(data);
		send_packet(packet);

		return true;
	}

	bool handler::send_move_packet(int x, int y) {
		

		std::vector<unsigned char> data;

		data.push_back(0x10);
		data.push_back(0x00);


		data.push_back((x) & 0xFF);
		data.push_back((x >> 8) & 0xFF);

		data.push_back((y) & 0xFF);
		data.push_back((y >> 8) & 0xFF);

		packet_buffer* packet = create_packet_buffer(data);
		send_packet(packet);

		return true;
	}

	bool handler::send_attack_packet(DWORD target_id) {
		

		std::vector<unsigned char> data;

		data.push_back(0x15);
		data.push_back(0x00);

		
		data.push_back((target_id) & 0xFF);
		data.push_back((target_id >> 8) & 0xFF);
		data.push_back((target_id >> 16) & 0xFF);
		data.push_back((target_id >> 24) & 0xFF);

		packet_buffer* packet = create_packet_buffer(data);
		send_packet(packet);

		return true;
	}

} 