#include "auth.h"
#include "manager.h"
#include "string_utils.h"
#include <stdio.h>

namespace network {


	std::string auth::s_session_token;
	std::string auth::s_username;
	std::string auth::s_password;

	void auth::set_credentials(const std::string& username, const std::string& password) {
		s_username = username;
		s_password = password;
	}

	bool auth::login(const std::string& username, const std::string& password) {
		pfn_game_login login_func = game::manager::get_login_func();

		if (!login_func) {

			return false;
		}


		game_wstring server;
		utils::init_game_wstring(&server, L"dreadmyst.com");

		game_wstring endpoint;
		utils::init_game_wstring(&endpoint, L"/auth/auth.php");

		game_string username_str;
		utils::init_game_string(&username_str, username.c_str());

		game_string password_str;
		utils::init_game_string(&password_str, password.c_str());

		game_string token;
		memset(&token, 0, sizeof(token));
		token.m_capacity = 15;

		game_string error;
		memset(&error, 0, sizeof(error));
		error.m_capacity = 15;

		char result = login_func(
			(WCHAR*)&server,
			(DWORD)&endpoint,
			&username_str,
			&password_str,
			&token,
			&error
		);

		if (result) {
			const char* token_str = (token.m_capacity > 15) ? token.m_ptr : token.m_buffer;
			s_session_token = token_str;
		}
		else {
			const char* error_str = (error.m_capacity > 15) ? error.m_ptr : error.m_buffer;
		}

		
		utils::free_game_string(&username_str);
		utils::free_game_string(&password_str);
		utils::free_game_string(&token);
		utils::free_game_string(&error);
		utils::free_game_wstring(&server);
		utils::free_game_wstring(&endpoint);

		return result != 0;
	}

	const std::string& auth::get_session_token() {
		return s_session_token;
	}

} 