#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "cbasetypes.hpp"
#include <string>


#define GAME_HOST       "game.dreadmyst.com"
#define GAME_PORT       16383
#define API_HOST        "dreadmyst.com"
#define API_PATH        "/auth/auth.php"
#define BUILD_VERSION   1206

struct Config {
    std::string username;
    std::string password;
    std::string character_name;
};

void config_init(void);
void config_final(void);

bool config_load(const char* filename);
bool config_save(const char* filename);

Config& config_get(void);

#endif