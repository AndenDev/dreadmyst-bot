#include "common.h"
#include "config.h"
#include "manager.h"
#include "auth.h"
#include "auto_login.h"
#include "controller.h"



BOOL APIENTRY DllMain(HMODULE h_module, DWORD ul_reason_for_call, LPVOID lp_reserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(h_module);

		
		AllocConsole();
		FILE* f_dummy;
		freopen_s(&f_dummy, "CONOUT$", "w", stdout);
		freopen_s(&f_dummy, "CONOUT$", "w", stderr);
		freopen_s(&f_dummy, "CONIN$", "r", stdin);

		SetConsoleTitleA("Dreadmyst Bot v1.0");

		printf("Loading configuration...\n");
		if (!config::settings::load()) {
			printf("Config file not found, creating default config.ini\n");
			config::settings::initialize_defaults();
			config::settings::save();
			printf("Please edit config.ini and restart\n");
			Sleep(5000);
			FreeConsole();
			return FALSE;
		}

		printf("Username: %s\n", config::settings::get_username().c_str());
		printf("Character ID: 0x%08X\n", config::settings::get_character_id());
		printf("\n");

		
		if (!game::manager::initialize()) {
			printf("[FATAL] Initialization failed!\n");
			Sleep(5000);
			FreeConsole();
			return FALSE;
		}

		printf("\nInitialization complete\n");
		printf("Auto-login will start in 3 seconds...\n\n");

		
		bot::auto_login::start(config::settings::get_character_id());

	
		if (config::settings::is_auto_attack_enabled() ||
			config::settings::is_auto_walk_enabled() ||
			config::settings::is_auto_loot_enabled()) {
			bot::controller::start();
			bot::controller::enable_auto_attack(config::settings::is_auto_attack_enabled());
			bot::controller::enable_auto_walk(config::settings::is_auto_walk_enabled());
			bot::controller::enable_auto_loot(config::settings::is_auto_loot_enabled());
		}

		break;

	case DLL_PROCESS_DETACH:
		bot::controller::stop();
		game::manager::shutdown();
		FreeConsole();
		break;
	}
	return TRUE;
}