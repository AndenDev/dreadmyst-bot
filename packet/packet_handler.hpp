#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include "../core/cbasetypes.hpp"
#include "../packet/packet_base.hpp"
#include <string>

class Session;

void packethandler_init(void);
void packethandler_final(void);


bool send_token(Session* session,
    const std::string& token,
    uint32 build_version = 1205,
    const std::string& fingerprint = "");

bool send_ping(Session* session, int64 timestamp = 0);
bool handle_validate(Session* session, PacketReader& reader);
bool handle_ping(Session* session, PacketReader& reader);


bool send_character_request_list(Session* session);
bool send_enter_world(Session* session, uint32 guid);
bool handle_character_list(Session* session, PacketReader& reader);
bool handle_enter_world(Session* session, PacketReader& reader);

#endif