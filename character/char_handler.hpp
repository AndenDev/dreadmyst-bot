
#ifndef CHAR_HANDLER_HPP
#define CHAR_HANDLER_HPP

#include "../core/cbasetypes.hpp"
#include "../packet/packet_base.hpp"
#include <string>
#include <map>

class Session;

void char_init(void);
void char_final(void);

bool char_send_request_list(Session* session);
bool char_send_enter_world(Session* session, uint32 guid);

bool char_handle_list(Session* session, PacketReader& reader);
bool char_handle_enter_world(Session* session, PacketReader& reader);

#endif