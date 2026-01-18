#ifndef DREADMYST_BOT_CONTROLLER_H
#define DREADMYST_BOT_CONTROLLER_H

#include "common.h"

namespace bot {

	class controller {
	public:
		static void start();
		static void stop();
		static bool is_running();

		static void enable_auto_attack(bool enable);
		static void enable_auto_walk(bool enable);
		static void enable_auto_loot(bool enable);

		static bool is_auto_attack_enabled();
		static bool is_auto_walk_enabled();
		static bool is_auto_loot_enabled();

	private:
		static bool s_running;
		static bool s_auto_attack;
		static bool s_auto_walk;
		static bool s_auto_loot;

		static DWORD WINAPI bot_thread(LPVOID lp_param);
	};

} 

#endif 