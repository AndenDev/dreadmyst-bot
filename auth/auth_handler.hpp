#ifndef AUTH_HANDLER_HPP
#define AUTH_HANDLER_HPP

#include "../core/cbasetypes.hpp"
#include "../packet/packet_base.hpp"
#include <string>

class Session;

void auth_init(void);
void auth_final(void);

bool auth_send_token(Session* session,
    const std::string& token,
    uint32 build_version = 1205,
    const std::string& fingerprint = "");

bool auth_send_ping(Session* session, int64 timestamp = 0);

bool auth_handle_validate(Session* session, PacketReader& reader);
bool auth_handle_ping(Session* session, PacketReader& reader);

#endif