#include "auto_login.h"
#include "auth.h"
#include "handler.h"
#include "manager.h"
#include "config.h"
#include <stdio.h>

namespace bot {

	DWORD auto_login::s_character_id = 0;

	DWORD WINAPI auto_login::auto_login_thread(LPVOID lp_param) {
		Sleep(3000); 

		execute();
		return 0;
	}

	void auto_login::start(DWORD character_id) {
		s_character_id = character_id;
		CreateThread(NULL, 0, auto_login_thread, NULL, 0, NULL);
	}

	bool auto_login::execute() {

		if (!network::auth::login(
			config::settings::get_username(),
			config::settings::get_password())) {
			return false;
		}

		Sleep(500);

		void* network_mgr = game::manager::get_network_manager();
		if (!network_mgr) {
			return false;
		}


		if (!network::handler::connect_to_server()) {

			return false;
		}


		Sleep(500);


		if (!network::handler::send_auth_packet(network::auth::get_session_token())) {
			return false;
		}

		Sleep(1500);


		network::handler::send_character_select(s_character_id);

		return true;
	}

} 