#include <iostream>
#include <chrono>
#include <thread>
#include "core/cbasetypes.hpp"
#include "core/showmsg.hpp"
#include "core/timer.hpp"
#include "network/socket.hpp"
#include "network/session.hpp"
#include "network/connector.hpp"
#include "packet/packet_db.hpp"
#include "auth/auth_handler.hpp"
#include "character/char_handler.hpp"

struct Config {
    std::string game_host = "game.dreadmyst.com";
    uint16      game_port = 16383;
    std::string api_host = "dreadmyst.com";
    std::string api_path = "/auth/auth.php";
    std::string username = "test";
    std::string password = "test";
    std::string charname = "test";
    uint32      build_version = 1206;
    std::string fingerprint = "";
};

bool wait_for(const std::function<bool()>& condition, int timeout_ms, Session* session)
{
    auto start = std::chrono::steady_clock::now();
    while (true) {
        if (!session->process())
            return false;
        if (condition())
            return true;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
        if (elapsed >= timeout_ms)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool fetch_character_guid(const Config& cfg, const std::string& token, uint32& out_guid)
{
    Session session;
    out_guid = 0;

    if (!session.connect(cfg.game_host, cfg.game_port)) {
        ShowError("Connection failed\n");
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!auth_send_token(&session, token, cfg.build_version, cfg.fingerprint)) {
        ShowError("Failed to send token\n");
        session.disconnect();
        return false;
    }

    if (!wait_for([&]() { return session.is_authenticated(); }, 5000, &session)) {
        ShowError("Authentication timeout\n");
        session.disconnect();
        return false;
    }

    if (!char_send_request_list(&session)) {
        ShowError("Failed to request character list\n");
        session.disconnect();
        return false;
    }

    if (!wait_for([&]() { return !session.characters().empty(); }, 5000, &session)) {
        ShowError("Character list timeout\n");
        session.disconnect();
        return false;
    }

    const CharacterInfo* ch = session.get_character_by_name(cfg.charname);
    if (!ch && !session.characters().empty())
        ch = &session.characters()[0];

    if (ch) {
        out_guid = ch->guid;
        ShowStatus("Character: %s (GUID: %u, Level: %d, Class: %u)\n",
            ch->name.c_str(), ch->guid, ch->level, ch->class_id);
    }

    session.disconnect();
    return out_guid != 0;
}

bool enter_world(const Config& cfg, const std::string& token, uint32 guid, Session& session)
{
    if (!session.connect(cfg.game_host, cfg.game_port)) {
        ShowError("Connection failed\n");
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!auth_send_token(&session, token, cfg.build_version, cfg.fingerprint)) {
        ShowError("Failed to send token\n");
        return false;
    }

    if (!wait_for([&]() { return session.is_authenticated(); }, 5000, &session)) {
        ShowError("Authentication timeout\n");
        return false;
    }

    if (!char_send_enter_world(&session, guid)) {
        ShowError("Failed to enter world\n");
        return false;
    }

    if (!wait_for([&]() { return session.is_in_world(); }, 10000, &session)) {
        ShowError("World entry timeout\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    Config cfg;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc)
            cfg.game_host = argv[++i];
        else if (arg == "--port" && i + 1 < argc)
            cfg.game_port = static_cast<uint16>(std::stoi(argv[++i]));
        else if (arg == "--user" && i + 1 < argc)
            cfg.username = argv[++i];
        else if (arg == "--pass" && i + 1 < argc)
            cfg.password = argv[++i];
        else if (arg == "--char" && i + 1 < argc)
            cfg.charname = argv[++i];
    }

    showmsg_init(true);
    timer_init();
    socket_init();
    packetdb().init();
    auth_init();
    char_init();

    Connector connector;
    std::string token, error;
    int result = 1;

    ShowStatus("Authenticating...\n");
    if (!connector.authenticate(cfg.api_host, cfg.api_path,
        cfg.username, cfg.password, token, error)) {
        ShowFatalError("Auth failed: %s\n", error.c_str());
        goto cleanup;
    }
    ShowStatus("Logged in as %s\n", cfg.username.c_str());

    uint32 char_guid;
    if (!fetch_character_guid(cfg, token, char_guid)) {
        ShowFatalError("No character found\n");
        goto cleanup;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        Session session;
        if (!enter_world(cfg, token, char_guid, session)) {
            ShowFatalError("Failed to enter world\n");
            goto cleanup;
        }

        ShowStatus("Entered world (Map: %d)\n",
            session.map_id());


        while (session.process()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        ShowStatus("Disconnected\n");
        session.disconnect();
    }

    result = 0;

cleanup:
    char_final();
    auth_final();
    packetdb().final();
    socket_final();
    timer_final();
    showmsg_final();
    return result;
}