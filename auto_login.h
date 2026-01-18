#ifndef DREADMYST_AUTO_LOGIN_H
#define DREADMYST_AUTO_LOGIN_H

#include "common.h"


namespace bot {

	class auto_login {
	public:
		static void start(DWORD character_id);
		static bool execute();

	private:
		static DWORD s_character_id;
		static DWORD WINAPI auto_login_thread(LPVOID lp_param);
	};

} 

#endif 