#include "auth_handler.hpp"
#include "../network/session.hpp"
#include "../packet/packet_db.hpp"
#include "../core/showmsg.hpp"
#include <cstring>

void auth_init(void)
{
    REGISTER_PACKET(OP_SERVER_VALIDATE, "SERVER_VALIDATE",
        PACKET_HEADER_SIZE + 1, auth_handle_validate);

    REGISTER_PACKET(OP_MUTUAL_PING, "MUTUAL_PING",
        PACKET_HEADER_SIZE, auth_handle_ping);

    REGISTER_PACKET(OP_CLIENT_AUTHENTICATE, "CLIENT_AUTHENTICATE",
        PACKET_HEADER_SIZE, nullptr);
}

void auth_final(void)
{
}

bool auth_send_token(Session* session,
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

bool auth_send_ping(Session* session, int64 timestamp)
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

bool auth_handle_ping(Session* session, PacketReader& reader)
{
    int64 timestamp = 0;
    if (reader.remaining() >= 8) {
        timestamp = reader.read_int64();
    }
    ShowInfo("Ping received\n");
    return auth_send_ping(session, timestamp);
}
bool auth_handle_validate(Session* session, PacketReader& reader)
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