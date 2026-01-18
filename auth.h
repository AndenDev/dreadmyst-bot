#ifndef DREADMYST_AUTH_H
#define DREADMYST_AUTH_H

#include "common.h"
#include <string>


namespace network {

	class auth {
	public:
		static bool login(const std::string& username, const std::string& password);
		static const std::string& get_session_token();
		static void set_credentials(const std::string& username, const std::string& password);

	private:
		static std::string s_session_token;
		static std::string s_username;
		static std::string s_password;
	};

} 

#endif 