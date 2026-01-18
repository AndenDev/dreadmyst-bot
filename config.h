#ifndef DREADMYST_CONFIG_H
#define DREADMYST_CONFIG_H

#include <string>


namespace config {

	class settings {
	public:

		static bool load_from_file(const char* filename);
		static bool save_to_file(const char* filename);
		static bool load();
		static bool save();

	
		static const std::string& get_username();
		static const std::string& get_password();
		static unsigned long get_character_id();


		
		static bool is_auto_attack_enabled();
		static bool is_auto_walk_enabled();
		static bool is_auto_loot_enabled();
		static unsigned int get_attack_delay();
		static unsigned int get_walk_delay();
		static unsigned int get_loot_delay();

	
		static void set_username(const std::string& username);
		static void set_password(const std::string& password);
		static void set_character_id(unsigned long character_id);


	
		static void set_auto_attack(bool enabled);
		static void set_auto_walk(bool enabled);
		static void set_auto_loot(bool enabled);
		static void set_attack_delay(unsigned int delay);
		static void set_walk_delay(unsigned int delay);
		static void set_loot_delay(unsigned int delay);

		
		static void initialize_defaults();

	private:

		static std::string s_username;
		static std::string s_password;
		static unsigned long s_character_id;


		static bool s_auto_attack_enabled;
		static bool s_auto_walk_enabled;
		static bool s_auto_loot_enabled;
		static unsigned int s_attack_delay;
		static unsigned int s_walk_delay;
		static unsigned int s_loot_delay;
	};

} 

#endif 