#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>

namespace config {

	
	std::string settings::s_username = "";
	std::string settings::s_password = "";
	unsigned long settings::s_character_id = 0;


	bool settings::s_auto_attack_enabled = false;
	bool settings::s_auto_walk_enabled = false;
	bool settings::s_auto_loot_enabled = false;
	unsigned int settings::s_attack_delay = 1000;
	unsigned int settings::s_walk_delay = 500;
	unsigned int settings::s_loot_delay = 300;

	void settings::initialize_defaults() {
		s_username = "YourUsername";
		s_password = "YourPassword";
		s_character_id = 0x00033644;


		s_auto_attack_enabled = false;
		s_auto_walk_enabled = false;
		s_auto_loot_enabled = false;
		s_attack_delay = 1000;
		s_walk_delay = 500;
		s_loot_delay = 300;
	}


	static std::string trim(const std::string& str) {
		size_t first = str.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) return "";
		size_t last = str.find_last_not_of(" \t\r\n");
		return str.substr(first, last - first + 1);
	}

	
	static bool parse_bool(const std::string& value) {
		std::string lower = value;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
		return (lower == "true" || lower == "1" || lower == "yes" || lower == "enabled");
	}

	bool settings::load_from_file(const char* filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			return false;
		}
		std::string line;
		std::string current_section;

		while (std::getline(file, line)) {

			line = trim(line);


			if (line.empty() || line[0] == ';' || line[0] == '#') {
				continue;
			}

			if (line[0] == '[' && line[line.length() - 1] == ']') {
				current_section = line.substr(1, line.length() - 2);
				continue;
			}

			size_t pos = line.find('=');
			if (pos == std::string::npos) {
				continue;
			}

			std::string key = trim(line.substr(0, pos));
			std::string value = trim(line.substr(pos + 1));

		
			if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
				value = value.substr(1, value.length() - 2);
			}

			if (current_section == "Account") {
				if (key == "username") s_username = value;
				else if (key == "password") s_password = value;
				else if (key == "character_id") {
					std::stringstream ss(value);
					if (value.find("0x") == 0 || value.find("0X") == 0) {
						ss >> std::hex >> s_character_id;
					}
					else {
						ss >> s_character_id;
					}
				}
			}
			else if (current_section == "Bot") {
				if (key == "auto_attack") s_auto_attack_enabled = parse_bool(value);
				else if (key == "auto_walk") s_auto_walk_enabled = parse_bool(value);
				else if (key == "auto_loot") s_auto_loot_enabled = parse_bool(value);
				else if (key == "attack_delay") s_attack_delay = std::stoul(value);
				else if (key == "walk_delay") s_walk_delay = std::stoul(value);
				else if (key == "loot_delay") s_loot_delay = std::stoul(value);
			}
		}

		file.close();
		return true;
	}

	bool settings::save_to_file(const char* filename) {
		std::ofstream file(filename);
		if (!file.is_open()) {
			return false;
		}

		file << "; Dreadmyst Bot Configuration File\n";
		file << "; Edit this file to change bot settings\n\n";

		file << "[Account]\n";
		file << "username = \"" << s_username << "\"\n";
		file << "password = \"" << s_password << "\"\n";
		file << "character_id = 0x" << std::hex << s_character_id << std::dec << "\n";
		file << "\n";

		file << "[Bot]\n";
		file << "; Enable/disable bot features (true/false)\n";
		file << "auto_attack = " << (s_auto_attack_enabled ? "true" : "false") << "\n";
		file << "auto_walk = " << (s_auto_walk_enabled ? "true" : "false") << "\n";
		file << "auto_loot = " << (s_auto_loot_enabled ? "true" : "false") << "\n";
		file << "\n";
		file << "; Delays in milliseconds\n";
		file << "attack_delay = " << s_attack_delay << "\n";
		file << "walk_delay = " << s_walk_delay << "\n";
		file << "loot_delay = " << s_loot_delay << "\n";

		file.close();
		return true;
	}

	bool settings::load() {
		return load_from_file("config.ini");
	}

	bool settings::save() {
		return save_to_file("config.ini");
	}

	
	const std::string& settings::get_username() { return s_username; }
	const std::string& settings::get_password() { return s_password; }
	unsigned long settings::get_character_id() { return s_character_id; }


	bool settings::is_auto_attack_enabled() { return s_auto_attack_enabled; }
	bool settings::is_auto_walk_enabled() { return s_auto_walk_enabled; }
	bool settings::is_auto_loot_enabled() { return s_auto_loot_enabled; }
	unsigned int settings::get_attack_delay() { return s_attack_delay; }
	unsigned int settings::get_walk_delay() { return s_walk_delay; }
	unsigned int settings::get_loot_delay() { return s_loot_delay; }


	void settings::set_username(const std::string& username) { s_username = username; }
	void settings::set_password(const std::string& password) { s_password = password; }
	void settings::set_character_id(unsigned long character_id) { s_character_id = character_id; }


	void settings::set_auto_attack(bool enabled) { s_auto_attack_enabled = enabled; }
	void settings::set_auto_walk(bool enabled) { s_auto_walk_enabled = enabled; }
	void settings::set_auto_loot(bool enabled) { s_auto_loot_enabled = enabled; }
	void settings::set_attack_delay(unsigned int delay) { s_attack_delay = delay; }
	void settings::set_walk_delay(unsigned int delay) { s_walk_delay = delay; }
	void settings::set_loot_delay(unsigned int delay) { s_loot_delay = delay; }

} 