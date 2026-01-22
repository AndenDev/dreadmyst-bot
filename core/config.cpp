#include "config.hpp"
#include "showmsg.hpp"
#include <fstream>

static Config g_config;

void config_init(void)
{
    g_config.username.clear();
    g_config.password.clear();
    g_config.character_name.clear();
}

void config_final(void)
{
}

Config& config_get(void)
{
    return g_config;
}

static std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool config_load(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        ShowError("Could not open config file: %s\n", filename);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/')
            continue;

        size_t pos = line.find(':');
        if (pos == std::string::npos)
            pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        if (key == "username")
            g_config.username = value;
        else if (key == "password")
            g_config.password = value;
        else if (key == "character_name")
            g_config.character_name = value;
    }

    file.close();
    ShowStatus("Config loaded: %s\n", filename);
    return true;
}

bool config_save(const char* filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        ShowError("Could not save config file: %s\n", filename);
        return false;
    }

    file << "# Bot Configuration\n";
    file << "username: " << g_config.username << "\n";
    file << "password: " << g_config.password << "\n";
    file << "character_name: " << g_config.character_name << "\n";

    file.close();
    ShowStatus("Config saved: %s\n", filename);
    return true;
}