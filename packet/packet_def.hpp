#ifndef PACKET_DEF_HPP
#define PACKET_DEF_HPP

#include "../core/cbasetypes.hpp"

#define PACKET_SIZE_FIELD       4
#define PACKET_OPCODE_SIZE      2
#define PACKET_HEADER_SIZE      6

#define PACKET_SIZE_MAX         0x00FFFFFF

enum PacketOpcode : opcode_t {
    OP_MUTUAL_PING = 1,

    OP_CLIENT_AUTHENTICATE = 2,
    OP_CLIENT_CHARACTER_LIST = 4,
    OP_CLIENT_ENTER_WORLD = 5,

    OP_SERVER_VALIDATE = 3,
    OP_SERVER_CHARACTER_LIST = 76,
    OP_SERVER_ENTER_WORLD = 77,
    OP_SERVER_PLAYER = 78,
    OP_SERVER_NPC = 79,
    OP_SERVER_GAMEOBJECT = 80,
};

enum AuthResult : uint8 {
    AUTH_VALIDATED = 0,
    AUTH_WRONG_VERSION = 1,
    AUTH_BAD_PASSWORD = 2,
    AUTH_SERVER_FULL = 3,
    AUTH_BANNED = 4,
};

inline uint32 PACKET_SIZE(const uint8* buf)
{
    return static_cast<uint32>(buf[0]) |
        (static_cast<uint32>(buf[1]) << 8) |
        (static_cast<uint32>(buf[2]) << 16) |
        (static_cast<uint32>(buf[3]) << 24);
}

inline opcode_t PACKET_OPCODE(const uint8* buf)
{
    return static_cast<opcode_t>(buf[4]) | (static_cast<opcode_t>(buf[5]) << 8);
}

#endif