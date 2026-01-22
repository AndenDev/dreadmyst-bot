#include <iostream>
#include <chrono>
#include <thread>
#include "core/cbasetypes.hpp"
#include "core/showmsg.hpp"
#include "core/timer.hpp"
#include "core/config.hpp"
#include "network/socket.hpp"
#include "network/session.hpp"
#include "network/connector.hpp"
#include "packet/packet_db.hpp"
#include "packet/packet_handler.hpp"

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

bool fetch_character_guid(const std::string& token, uint32& out_guid)
{
    Config& cfg = config_get();
    Session session;
    out_guid = 0;

    if (!session.connect(GAME_HOST, GAME_PORT)) {
        ShowError("Connection failed\n");
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!send_token(&session, token, BUILD_VERSION, "")) {
        ShowError("Failed to send token\n");
        session.disconnect();
        return false;
    }

    if (!wait_for([&]() { return session.is_authenticated(); }, 5000, &session)) {
        ShowError("Authentication timeout\n");
        session.disconnect();
        return false;
    }

    if (!send_character_request_list(&session)) {
        ShowError("Failed to request character list\n");
        session.disconnect();
        return false;
    }

    if (!wait_for([&]() { return !session.characters().empty(); }, 5000, &session)) {
        ShowError("Character list timeout\n");
        session.disconnect();
        return false;
    }

    const CharacterInfo* ch = session.get_character_by_name(cfg.character_name);
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

bool enter_world(const std::string& token, uint32 guid, Session& session)
{
    if (!session.connect(GAME_HOST, GAME_PORT)) {
        ShowError("Connection failed\n");
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!send_token(&session, token, BUILD_VERSION, "")) {
        ShowError("Failed to send token\n");
        return false;
    }

    if (!wait_for([&]() { return session.is_authenticated(); }, 5000, &session)) {
        ShowError("Authentication timeout\n");
        return false;
    }

    if (!send_enter_world(&session, guid)) {
        ShowError("Failed to enter world\n");
        return false;
    }

    if (!wait_for([&]() { return session.is_in_world(); }, 10000, &session)) {
        ShowError("World entry timeout\n");
        return false;
    }

    return true;
}

int main()
{
    showmsg_init(true);
    config_init();
    timer_init();
    socket_init();
    packetdb().init();
    packethandler_init();

    Config& cfg = config_get();

 
    config_load("bot.conf");

    Connector connector;
    std::string token, error;
    int result = 1;

    ShowStatus("Authenticating...\n");
    if (!connector.authenticate(API_HOST, API_PATH,
        cfg.username, cfg.password, token, error)) {
        ShowFatalError("Auth failed: %s\n", error.c_str());
        goto cleanup;
    }
    ShowStatus("Logged in as %s\n", cfg.username.c_str());

    uint32 char_guid;
    if (!fetch_character_guid(token, char_guid)) {
        ShowFatalError("No character found\n");
        goto cleanup;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        Session session;
        if (!enter_world(token, char_guid, session)) {
            ShowFatalError("Failed to enter world\n");
            goto cleanup;
        }

        ShowStatus("Entered world (Map: %d)\n", session.map_id());

        while (session.process()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        ShowStatus("Disconnected\n");
        session.disconnect();
    }

    result = 0;

cleanup:
    packethandler_final();
    packetdb().final();
    socket_final();
    timer_final();
    config_final();
    showmsg_final();
    return result;
}