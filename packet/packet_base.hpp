#ifndef PACKET_BASE_HPP
#define PACKET_BASE_HPP

#include "../core/cbasetypes.hpp"
#include "packet_def.hpp"
#include <string>
#include <vector>
#include <cstring>

class PacketBuffer {
public:
    PacketBuffer();
    explicit PacketBuffer(opcode_t opcode);
    ~PacketBuffer();

    PacketBuffer(const PacketBuffer&) = delete;
    PacketBuffer& operator=(const PacketBuffer&) = delete;
    PacketBuffer(PacketBuffer&& other) noexcept;
    PacketBuffer& operator=(PacketBuffer&& other) noexcept;

    void init(opcode_t opcode);

    const uint8* data(void) const { return m_buffer.data(); }
    size_t size(void) const { return m_buffer.size(); }

    void write_uint32(uint32 value);
    void finalize(void);
    bool is_finalized(void) const { return m_finalized; }

private:
    std::vector<uint8> m_buffer;
    opcode_t m_opcode;
    bool m_finalized;
};

class PacketReader {
public:
    PacketReader();
    PacketReader(const uint8* data, size_t len);
    ~PacketReader();

    void init(const uint8* data, size_t len);

    size_t remaining(void) const;
    void skip(size_t count);

    uint8  read_uint8(void);
    uint16 read_uint16(void);
    int16  read_int16(void);
    uint32 read_uint32(void);
    int32  read_int32(void);
    int64  read_int64(void);
    float  read_float(void);

    std::string read_string(void);
    std::string read_cstring(void);

private:
    const uint8* m_data;
    size_t m_len;
    size_t m_pos;

    void check_bounds(size_t count) const;
};

#endif