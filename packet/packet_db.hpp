#ifndef PACKET_DB_HPP
#define PACKET_DB_HPP

#include "../core/cbasetypes.hpp"
#include "../core/showmsg.hpp"
#include "packet_def.hpp"
#include "packet_base.hpp"
#include <functional>
#include <unordered_map>
#include <string>

class Session;

using PacketHandler = std::function<bool(Session* session, PacketReader& reader)>;

struct PacketInfo {
    opcode_t      opcode;
    std::string   name;
    int           min_size;
    PacketHandler handler;
};

class PacketDB {
public:
    static PacketDB& instance(void);

    PacketDB(const PacketDB&) = delete;
    PacketDB& operator=(const PacketDB&) = delete;

    void init(void);
    void final(void);

    void register_packet(opcode_t opcode, const char* name, int min_size, PacketHandler handler);
    bool dispatch(Session* session, const uint8* data, size_t len);

private:
    PacketDB();
    ~PacketDB();

    std::unordered_map<opcode_t, PacketInfo> m_packets;
};

#define REGISTER_PACKET(opcode, name, min_size, handler) \
    PacketDB::instance().register_packet(opcode, name, min_size, handler)

static inline PacketDB& packetdb(void)
{
    return PacketDB::instance();
}

#endif