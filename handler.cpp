#include "handler.h"
#include "packets.h"
#include "manager.h"
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <intrin.h>
#include <random>

namespace network {

	std::string handler::generate_random_fingerprint() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);

		std::stringstream result;
		result << std::hex << std::setfill('0');
		
		for (int i = 0; i < 32; i++) {
			result << std::hex << dis(gen);
		}

		std::string fingerprint = result.str();
		printf("[HWID] Generated random fingerprint: %s\n", fingerprint.c_str());
		
		return fingerprint;
	}

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
			printf("[PACKET] Failed to get network manager\n");
			free(packet->m_data_start);
			free(packet);
			return;
		}

		pfn_game_send_packet send_func = game::manager::get_send_packet_func();
		if (send_func) {
			send_func(network_mgr, packet);
			printf("[PACKET] Sent packet (%zu bytes)\n", packet->m_data_end - packet->m_data_start);
		}

		free(packet->m_data_start);
		free(packet);
	}

	bool handler::connect_to_server() {
		void* network_mgr = game::manager::get_network_manager();
		if (!network_mgr) {
			printf("[CONNECT] Failed to get network manager\n");
			return false;
		}

		DWORD base_addr = game::manager::get_base_address();
		DWORD connect_func = base_addr + 0x26080;
		char result = 0;

		printf("[CONNECT] Attempting connection to game server...\n");

		__asm {
			push ebp
			mov ecx, network_mgr
			mov eax, connect_func
			call eax
			mov result, al
			pop ebp
		}

		if (result) {
			printf("[CONNECT] Successfully connected to game server\n");
		}
		else {
			printf("[CONNECT] Failed to connect to game server\n");
		}

		return result != 0;
	}

	//bool handler::send_auth_packet(const std::string& token) {
	//	if (token.empty()) {
	//		printf("[AUTH PACKET] Token is empty\n");
	//		return false;
	//	}
	//	
	//	auth_packet pkt = {};
	//	pkt.packet_id = PACKET_AUTH;
	//	
	//	strncpy_s(pkt.token, sizeof(pkt.token), token.c_str(), _TRUNCATE);
	//	
	//	pkt.build_version = 0x04AD;
	//	
	//	std::string hwid = generate_random_fingerprint();
	//	strncpy_s(pkt.hardware_id, sizeof(pkt.hardware_id), hwid.c_str(), _TRUNCATE);

	//	printf("[AUTH PACKET] Sending authentication packet\n");
	//	printf("[AUTH PACKET] Token: %s\n", token.c_str());
	//	printf("[AUTH PACKET] Build: 1197 (0x04AD)\n");
	//	printf("[AUTH PACKET] Hardware ID: %s\n", hwid.c_str());

	//	size_t token_len = strlen(pkt.token) + 1;
	//	size_t hwid_len = strlen(pkt.hardware_id) + 1;
	//	size_t packet_size = sizeof(WORD) + token_len + sizeof(DWORD) + hwid_len;

	//	packet_buffer* packet = create_packet_buffer(&pkt, packet_size);
	//	send_packet(packet);

	//	return true;
	//}

	//bool handler::send_character_select(DWORD character_id) {
	//	printf("[CHAR SELECT] Sending character select packet (ID: 0x%08X)\n", character_id);

	//	character_select_packet pkt = {};
	//	pkt.packet_id = PACKET_CHARACTER_SELECT;
	//	pkt.character_id = character_id;

	//	packet_buffer* packet = create_packet_buffer(&pkt, sizeof(pkt));
	//	send_packet(packet);

	//	return true;
	//}



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


		std::string hardware_id = generate_random_fingerprint();
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
}
