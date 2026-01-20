#include "auth.h"
#include "manager.h"
#include <stdio.h>

namespace network {

	std::string auth::s_session_token;
	std::string auth::s_last_error;

	const std::string& auth::get_session_token() {
		return s_session_token;
	}

	const std::string& auth::get_last_error() {
		return s_last_error;
	}

	bool auth::login(const std::string& username, const std::string& password) {
		printf("[AUTH] Attempting authentication for user: %s\n", username.c_str());

		pfn_game_login login_func = game::manager::get_login_func();
		if (!login_func) {
			s_last_error = "Game login function not found";
			printf("[AUTH] Failed: %s\n", s_last_error.c_str());
			return false;
		}

	
		const wchar_t* server = L"dreadmyst.com";
		const wchar_t* endpoint = L"/auth/auth.php";

		char token_buffer[256] = { 0 };
		char error_buffer[256] = { 0 };

		
		char result = login_func(
			(WCHAR*)server,
			(DWORD)endpoint,
			(void*)username.c_str(),
			(void*)password.c_str(),
			(void*)token_buffer,
			(void*)error_buffer
		);

		if (result) {
			s_session_token = token_buffer;
			s_last_error.clear();
			printf("[AUTH] Authentication successful! Token: %s\n", s_session_token.c_str());
		}
		else {
			s_last_error = error_buffer;
			s_session_token.clear();
			printf("[AUTH] Authentication failed: %s\n", s_last_error.c_str());
		}

		return result != 0;
	}

}
