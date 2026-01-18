#include "manager.h"
#include <stdio.h>

namespace game {

	DWORD manager::s_game_base_address = 0;
	pfn_game_send_packet manager::s_game_send_packet = nullptr;
	pfn_game_login manager::s_game_login = nullptr;
	pfn_get_network_manager manager::s_get_network_manager = nullptr;

	bool manager::initialize() {
		s_game_base_address = (DWORD)GetModuleHandleA("Dreadmyst.exe");
		if (!s_game_base_address) {
			return false;
		}

		s_game_send_packet = (pfn_game_send_packet)(s_game_base_address + 0x24D20);
		s_game_login = (pfn_game_login)(s_game_base_address + 0x24FE0);
		s_get_network_manager = (pfn_get_network_manager)(s_game_base_address + 0x4180);

		return true;
	}

	void manager::shutdown() {
		s_game_send_packet = nullptr;
		s_game_login = nullptr;
		s_get_network_manager = nullptr;
		s_game_base_address = 0;
	}

	DWORD manager::get_base_address() {
		return s_game_base_address;
	}

	void* manager::get_network_manager() {
		if (s_get_network_manager == nullptr) {
			return nullptr;
		}
		return s_get_network_manager();
	}

	pfn_game_send_packet manager::get_send_packet_func() {
		return s_game_send_packet;
	}

	pfn_game_login manager::get_login_func() {
		return s_game_login;
	}

	pfn_get_network_manager manager::get_network_manager_func() {
		return s_get_network_manager;
	}

} 