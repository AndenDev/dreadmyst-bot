#include "char_handler.hpp"
#include "../network/session.hpp"
#include "../packet/packet_db.hpp"
#include "../core/showmsg.hpp"
#include <map>

void char_init(void)
{
    REGISTER_PACKET(OP_SERVER_CHARACTER_LIST, "SERVER_CHARACTER_LIST",
        PACKET_HEADER_SIZE + 2, char_handle_list);

    REGISTER_PACKET(OP_SERVER_ENTER_WORLD, "SERVER_ENTER_WORLD",
        PACKET_HEADER_SIZE + 1, char_handle_enter_world);
}

void char_final(void)
{
}

bool char_send_request_list(Session* session)
{
    if (session == nullptr || !session->is_connected())
        return false;
    if (!session->is_authenticated())
        return false;

    PacketBuffer packet(OP_CLIENT_CHARACTER_LIST);
    packet.finalize();
    return session->send_packet(packet);
}

bool char_send_enter_world(Session* session, uint32 guid)
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

bool char_handle_list(Session* session, PacketReader& reader)
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

bool char_handle_enter_world(Session* session, PacketReader& reader)
{
    if (reader.remaining() < 1)
        return false;

    uint8 map_id = reader.read_uint8();

    session->set_map_id(map_id);
    session->set_in_world(true);
    session->set_state(SESSION_STATE_INGAME);

    return true;
}
