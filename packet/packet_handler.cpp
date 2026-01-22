#include "packet_handler.hpp"
#include "../network/session.hpp"
#include "../packet/packet_db.hpp"
#include "../core/showmsg.hpp"
#include <cstring>

void packethandler_init(void)
{
    REGISTER_PACKET(OP_SERVER_VALIDATE, "SERVER_VALIDATE",
        PACKET_HEADER_SIZE + 1, handle_validate);

    REGISTER_PACKET(OP_MUTUAL_PING, "MUTUAL_PING",
        PACKET_HEADER_SIZE, handle_ping);

    REGISTER_PACKET(OP_CLIENT_AUTHENTICATE, "CLIENT_AUTHENTICATE",
        PACKET_HEADER_SIZE, nullptr);

    REGISTER_PACKET(OP_SERVER_CHARACTER_LIST, "SERVER_CHARACTER_LIST",
        PACKET_HEADER_SIZE + 2, handle_character_list);

    REGISTER_PACKET(OP_SERVER_ENTER_WORLD, "SERVER_ENTER_WORLD",
        PACKET_HEADER_SIZE + 1, handle_enter_world);
}

void packethandler_final(void)
{
}

bool send_token(Session* session,
    const std::string& token,
    uint32 build_version,
    const std::string& fingerprint)
{
    if (session == nullptr || !session->is_connected())
        return false;

    const size_t PAYLOAD_SIZE = 2 + 65 + 4 + 33;
    const size_t PACKET_SIZE_TOTAL = 4 + PAYLOAD_SIZE;

    uint8 packet[PACKET_SIZE_TOTAL];
    memset(packet, 0, sizeof(packet));
    size_t pos = 0;

    packet[pos++] = static_cast<uint8>(PAYLOAD_SIZE & 0xFF);
    packet[pos++] = static_cast<uint8>((PAYLOAD_SIZE >> 8) & 0xFF);
    packet[pos++] = static_cast<uint8>((PAYLOAD_SIZE >> 16) & 0xFF);
    packet[pos++] = static_cast<uint8>((PAYLOAD_SIZE >> 24) & 0xFF);

    packet[pos++] = 0x02;
    packet[pos++] = 0x00;

    size_t token_len = token.length();
    if (token_len > 64) token_len = 64;
    memcpy(packet + pos, token.c_str(), token_len);
    pos += 65;

    packet[pos++] = static_cast<uint8>(build_version & 0xFF);
    packet[pos++] = static_cast<uint8>((build_version >> 8) & 0xFF);
    packet[pos++] = static_cast<uint8>((build_version >> 16) & 0xFF);
    packet[pos++] = static_cast<uint8>((build_version >> 24) & 0xFF);

    if (!fingerprint.empty()) {
        size_t fp_len = fingerprint.length();
        if (fp_len > 32) fp_len = 32;
        memcpy(packet + pos, fingerprint.c_str(), fp_len);
    }
    pos += 33;

    if (!session->send_raw(packet, pos))
        return false;

    session->set_state(SESSION_STATE_AUTHENTICATING);
    return true;
}

bool send_ping(Session* session, int64 timestamp)
{
    if (session == nullptr || !session->is_connected())
        return false;

    uint8 packet[14];

    packet[0] = 0x0a;
    packet[1] = 0x00;
    packet[2] = 0x00;
    packet[3] = 0x00;

    packet[4] = 0x01;
    packet[5] = 0x00;

    memcpy(packet + 6, &timestamp, 8);

    return session->send_raw(packet, 14);
}

bool handle_ping(Session* session, PacketReader& reader)
{
    int64 timestamp = 0;
    if (reader.remaining() >= 8) {
        timestamp = reader.read_int64();
    }
    ShowInfo("Ping received (timestamp: %lld)\n", timestamp);
    return send_ping(session, timestamp);
}

bool handle_validate(Session* session, PacketReader& reader)
{
    uint32 result = reader.read_uint32();

    int64 server_time = 0;
    if (reader.remaining() >= 8) {
        server_time = reader.read_int64();
    }

    if (result == AUTH_VALIDATED) {
        session->set_server_time(server_time);
        session->set_authenticated(true);
        session->set_state(SESSION_STATE_AUTHENTICATED);
        return true;
    }

    return false;
}


bool send_character_request_list(Session* session)
{
    if (session == nullptr || !session->is_connected())
        return false;
    if (!session->is_authenticated())
        return false;

    PacketBuffer packet(OP_CLIENT_CHARACTER_LIST);
    packet.finalize();
    return session->send_packet(packet);
}

bool send_enter_world(Session* session, uint32 guid)
{
    if (session == nullptr || !session->is_connected())
        return false;
    if (!session->is_authenticated())
        return false;

    PacketBuffer packet(OP_CLIENT_ENTER_WORLD);
    packet.write_uint32(guid);
    packet.finalize();
    return session->send_packet(packet);
}

bool handle_character_list(Session* session, PacketReader& reader)
{
    uint16 count = reader.read_uint16();

    session->clear_characters();
    auto& characters = session->characters();
    characters.reserve(count);

    for (uint16 i = 0; i < count; i++) {
        CharacterInfo info;

        uint16 unknown = reader.read_uint16();
        info.name = reader.read_cstring();
        info.class_id = reader.read_uint8();
        info.level = reader.read_uint8();
        info.guid = reader.read_uint32();
        uint8 portrait = reader.read_uint8();
        info.gender = reader.read_uint8();

        characters.push_back(info);
    }

    session->set_state(SESSION_STATE_CHARSELECT);
    return true;
}

bool handle_enter_world(Session* session, PacketReader& reader)
{
    if (reader.remaining() < 1)
        return false;

    uint8 map_id = reader.read_uint8();

    session->set_map_id(map_id);
    session->set_in_world(true);
    session->set_state(SESSION_STATE_INGAME);

    return true;
}