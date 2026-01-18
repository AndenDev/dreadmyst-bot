#ifndef DREADMYST_GAME_MANAGER_H
#define DREADMYST_GAME_MANAGER_H

#include "common.h"


namespace game {

	class manager {
	public:
		static bool initialize();
		static void shutdown();

		static DWORD get_base_address();
		static void* get_network_manager();

		static pfn_game_send_packet get_send_packet_func();
		static pfn_game_login get_login_func();
		static pfn_get_network_manager get_network_manager_func();

	private:
		static DWORD s_game_base_address;
		static pfn_game_send_packet s_game_send_packet;
		static pfn_game_login s_game_login;
		static pfn_get_network_manager s_get_network_manager;
	};

} 

#endif 