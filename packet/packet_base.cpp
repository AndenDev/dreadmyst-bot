#include "packet_base.hpp"
#include <stdexcept>

PacketBuffer::PacketBuffer()
    : m_opcode(0)
    , m_finalized(false)
{
    m_buffer.reserve(64);
}

PacketBuffer::PacketBuffer(opcode_t opcode)
    : m_opcode(opcode)
    , m_finalized(false)
{
    m_buffer.reserve(64);
    init(opcode);
}

PacketBuffer::~PacketBuffer()
{
}

PacketBuffer::PacketBuffer(PacketBuffer&& other) noexcept
    : m_buffer(std::move(other.m_buffer))
    , m_opcode(other.m_opcode)
    , m_finalized(other.m_finalized)
{
    other.m_opcode = 0;
    other.m_finalized = false;
}

PacketBuffer& PacketBuffer::operator=(PacketBuffer&& other) noexcept
{
    if (this != &other) {
        m_buffer = std::move(other.m_buffer);
        m_opcode = other.m_opcode;
        m_finalized = other.m_finalized;
        other.m_opcode = 0;
        other.m_finalized = false;
    }
    return *this;
}

void PacketBuffer::init(opcode_t opcode)
{
    m_buffer.clear();
    m_opcode = opcode;
    m_finalized = false;
    m_buffer.push_back(static_cast<uint8>(opcode & 0xFF));
    m_buffer.push_back(static_cast<uint8>((opcode >> 8) & 0xFF));
}

void PacketBuffer::write_uint32(uint32 value)
{
    m_buffer.push_back(static_cast<uint8>(value & 0xFF));
    m_buffer.push_back(static_cast<uint8>((value >> 8) & 0xFF));
    m_buffer.push_back(static_cast<uint8>((value >> 16) & 0xFF));
    m_buffer.push_back(static_cast<uint8>((value >> 24) & 0xFF));
}

void PacketBuffer::finalize(void)
{
    if (m_finalized)
        return;

    uint32 size_value = static_cast<uint32>(m_buffer.size());

    std::vector<uint8> final_buffer;
    final_buffer.reserve(PACKET_SIZE_FIELD + m_buffer.size());

    final_buffer.push_back(static_cast<uint8>(size_value & 0xFF));
    final_buffer.push_back(static_cast<uint8>((size_value >> 8) & 0xFF));
    final_buffer.push_back(static_cast<uint8>((size_value >> 16) & 0xFF));
    final_buffer.push_back(static_cast<uint8>((size_value >> 24) & 0xFF));

    final_buffer.insert(final_buffer.end(), m_buffer.begin(), m_buffer.end());

    m_buffer = std::move(final_buffer);
    m_finalized = true;
}

// PacketReader

PacketReader::PacketReader()
    : m_data(nullptr)
    , m_len(0)
    , m_pos(0)
{
}

PacketReader::PacketReader(const uint8* data, size_t len)
    : m_data(data)
    , m_len(len)
    , m_pos(PACKET_HEADER_SIZE)
{
}

PacketReader::~PacketReader()
{
}

void PacketReader::init(const uint8* data, size_t len)
{
    m_data = data;
    m_len = len;
    m_pos = PACKET_HEADER_SIZE;
}

size_t PacketReader::remaining(void) const
{
    return (m_pos < m_len) ? (m_len - m_pos) : 0;
}

void PacketReader::skip(size_t count)
{
    m_pos += count;
    if (m_pos > m_len) m_pos = m_len;
}

void PacketReader::check_bounds(size_t count) const
{
    if (m_pos + count > m_len) {
        throw std::runtime_error("PacketReader: Read past end of packet");
    }
}

uint8 PacketReader::read_uint8(void)
{
    check_bounds(1);
    return m_data[m_pos++];
}

uint16 PacketReader::read_uint16(void)
{
    check_bounds(2);
    uint16 value = static_cast<uint16>(m_data[m_pos] | (m_data[m_pos + 1] << 8));
    m_pos += 2;
    return value;
}

int16 PacketReader::read_int16(void)
{
    return static_cast<int16>(read_uint16());
}

uint32 PacketReader::read_uint32(void)
{
    check_bounds(4);
    uint32 value = static_cast<uint32>(
        m_data[m_pos] |
        (m_data[m_pos + 1] << 8) |
        (m_data[m_pos + 2] << 16) |
        (m_data[m_pos + 3] << 24)
        );
    m_pos += 4;
    return value;
}

int32 PacketReader::read_int32(void)
{
    return static_cast<int32>(read_uint32());
}

int64 PacketReader::read_int64(void)
{
    check_bounds(8);
    int64 value = 0;
    for (int i = 0; i < 8; i++) {
        value |= (static_cast<int64>(m_data[m_pos + i])) << (i * 8);
    }
    m_pos += 8;
    return value;
}

float PacketReader::read_float(void)
{
    uint32 bits = read_uint32();
    float value;
    memcpy(&value, &bits, sizeof(float));
    return value;
}

std::string PacketReader::read_cstring(void)
{
    std::string result;
    while (m_pos < m_len) {
        uint8 c = m_data[m_pos++];
        if (c == 0)
            break;
        result.push_back(static_cast<char>(c));
    }
    return result;
}

std::string PacketReader::read_string(void)
{
    uint16 length = read_uint16();
    if (length == 0)
        return "";

    check_bounds(length);
    std::string result(reinterpret_cast<const char*>(m_data + m_pos), length);
    m_pos += length;
    return result;
}