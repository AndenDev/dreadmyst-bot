#include "handler.h"
#include "packets.h"
#include "manager.h"
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <intrin.h>

namespace network {

	std::string handler::get_machine_fingerprint() {
		static std::string cached_fingerprint;
		
		if (!cached_fingerprint.empty()) {
			return cached_fingerprint;
		}

		std::ostringstream out;

		
		char user[256] = {};
		DWORD userLen = sizeof(user);
		GetUserNameA(user, &userLen);

		
		char computer[256] = {};
		DWORD compLen = sizeof(computer);
		GetComputerNameA(computer, &compLen);

		
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		
		int cpuInfo[4] = {};
		__cpuid(cpuInfo, 0);
		char vendor[13];
		memcpy(vendor + 0, &cpuInfo[1], 4);
		memcpy(vendor + 4, &cpuInfo[3], 4);
		memcpy(vendor + 8, &cpuInfo[2], 4);
		vendor[12] = 0;

		
		char brand[49] = {};
		__cpuid(cpuInfo, 0x80000002);
		memcpy(brand, cpuInfo, 16);
		__cpuid(cpuInfo, 0x80000003);
		memcpy(brand + 16, cpuInfo, 16);
		__cpuid(cpuInfo, 0x80000004);
		memcpy(brand + 32, cpuInfo, 16);

		
		MEMORYSTATUSEX ms = {};
		ms.dwLength = sizeof(ms);
		GlobalMemoryStatusEx(&ms);
		DWORD memRounded = (DWORD)((ms.ullTotalPhys / (1024ULL * 1024 * 512)) * 512);

		
		out << user << "|"
			<< computer << "|"
			<< si.dwNumberOfProcessors << "|"
			<< vendor << "|"
			<< brand << "|"
			<< memRounded;

		std::string fingerprint_data = out.str();

		
		unsigned int hash = 0x12345678;
		for (size_t i = 0; i < fingerprint_data.length(); i++) {
			hash = ((hash << 5) + hash) + fingerprint_data[i];
		}

		
		std::stringstream result;
		result << std::hex << std::setfill('0');
		for (int i = 0; i < 8; i++) {
			unsigned int segment = hash * (i + 1) + fingerprint_data.length() * (i + 7);
			result << std::setw(4) << (segment & 0xFFFF);
		}

		cached_fingerprint = result.str();
		printf("[HWID] Generated hardware fingerprint: %s\n", cached_fingerprint.c_str());

		return cached_fingerprint;
	}

	packet_buffer* handler::create_packet_buffer(void* data, size_t size) {
		packet_buffer* packet = (packet_buffer*)malloc(sizeof(packet_buffer));
		memset(packet, 0, sizeof(packet_buffer));

		char* buffer = (char*)malloc(size);
		memcpy(buffer, data, size);

		packet->m_data_start = buffer;
		packet->m_data_end = buffer + size;

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
		DWORD connect_func = base_addr + 0x24DB0;
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

	bool handler::send_auth_packet(const std::string& token) {
		if (token.empty()) {
			printf("[AUTH PACKET] Token is empty\n");
			return false;
		}

		auth_packet pkt = {};
		pkt.packet_id = PACKET_AUTH;
		
		strncpy_s(pkt.token, sizeof(pkt.token), token.c_str(), _TRUNCATE);
		pkt.build_version = 0x04AD;
		
		std::string hwid = get_machine_fingerprint();
		strncpy_s(pkt.hardware_id, sizeof(pkt.hardware_id), hwid.c_str(), _TRUNCATE);

		printf("[AUTH PACKET] Sending authentication packet\n");
		printf("[AUTH PACKET] Token: %s\n", token.c_str());
		printf("[AUTH PACKET] Build: 1197 (0x04AD)\n");
		printf("[AUTH PACKET] Hardware ID: %s\n", hwid.c_str());

		size_t token_len = strlen(pkt.token) + 1;  
		size_t hwid_len = strlen(pkt.hardware_id) + 1; 
		size_t packet_size = sizeof(WORD) + token_len + sizeof(DWORD) + hwid_len;

	
		packet_buffer* packet = create_packet_buffer(&pkt, packet_size);
		send_packet(packet);

		return true;
	}

	bool handler::send_character_select(DWORD character_id) {
		printf("[CHAR SELECT] Sending character select packet (ID: 0x%08X)\n", character_id);

		character_select_packet pkt = {};
		pkt.packet_id = PACKET_CHARACTER_SELECT;
		pkt.character_id = character_id;

		packet_buffer* packet = create_packet_buffer(&pkt, sizeof(pkt));
		send_packet(packet);

		return true;
	}

}
