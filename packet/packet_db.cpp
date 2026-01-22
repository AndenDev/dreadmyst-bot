#include "packet_db.hpp"

PacketDB::PacketDB()
{
}

PacketDB::~PacketDB()
{
}

PacketDB& PacketDB::instance(void)
{
    static PacketDB s_instance;
    return s_instance;
}

void PacketDB::init(void)
{
    m_packets.clear();
}

void PacketDB::final(void)
{
    m_packets.clear();
}

void PacketDB::register_packet(opcode_t opcode, const char* name, int min_size, PacketHandler handler)
{
    PacketInfo info;
    info.opcode = opcode;
    info.name = name ? name : "";
    info.min_size = min_size;
    info.handler = handler;
    m_packets[opcode] = info;
}

bool PacketDB::dispatch(Session* session, const uint8* data, size_t len)
{
    if (data == nullptr || len < PACKET_HEADER_SIZE)
        return false;

    opcode_t opcode = PACKET_OPCODE(data);
    uint32 size = PACKET_SIZE(data);

    auto it = m_packets.find(opcode);
    if (it == m_packets.end()) {
        return false;
    }

    const PacketInfo& info = it->second;

    if (info.min_size > 0 && static_cast<int>(len) < info.min_size)
        return false;

    if (!info.handler)
        return true;

    PacketReader reader(data, len);
    return info.handler(session, reader);
}