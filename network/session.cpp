#include "session.hpp"
#include "../packet/packet_db.hpp"
#include "../core/showmsg.hpp"
#include <cstring>

Session::Session()
    : m_state(SESSION_STATE_DISCONNECTED)
    , m_port(0)
    , m_authenticated(false)
    , m_server_time(0)
    , m_in_world(false)
    , m_map_id(0)
    , m_controlled_guid(0)
    , m_x(0.0f)
    , m_y(0.0f)
    , m_last_recv(0)
    , m_last_send(0)
{
    m_recv_buffer.reserve(RECV_BUFFER_SIZE);
}

Session::~Session()
{
    disconnect();
}

bool Session::connect(const std::string& host, uint16 port)
{
    if (is_connected())
        disconnect();

    m_host = host;
    m_port = port;
    m_state = SESSION_STATE_CONNECTING;

    if (!m_socket.create()) {
        m_state = SESSION_STATE_DISCONNECTED;
        return false;
    }

    if (!m_socket.connect(host, port)) {
        m_socket.close();
        m_state = SESSION_STATE_DISCONNECTED;
        return false;
    }

    m_socket.set_nodelay(true);
    m_socket.set_recv_timeout(10000);

    m_state = SESSION_STATE_CONNECTED;
    m_last_recv = gettick();
    m_last_send = gettick();

    return true;
}

void Session::disconnect(void)
{
    if (m_state == SESSION_STATE_DISCONNECTED)
        return;

    m_state = SESSION_STATE_DISCONNECTING;
    m_socket.close();
    m_recv_buffer.clear();
    m_characters.clear();
    m_authenticated = false;
    m_server_time = 0;
    m_in_world = false;
    m_map_id = 0;
    m_controlled_guid = 0;
    m_x = 0.0f;
    m_y = 0.0f;
    m_state = SESSION_STATE_DISCONNECTED;
}

bool Session::process(void)
{
    if (!is_connected())
        return false;

    if (m_socket.poll_read(10)) {
        uint8 buffer[4096];
        int received = m_socket.recv(buffer, sizeof(buffer));

        if (received <= 0) {
            disconnect();
            return false;
        }

        m_recv_buffer.insert(m_recv_buffer.end(), buffer, buffer + received);
        m_last_recv = gettick();

        process_recv();
    }

    return is_connected();
}

bool Session::process_recv(void)
{
    bool processed = false;

    while (m_recv_buffer.size() >= PACKET_HEADER_SIZE) {
        uint32 size_field = PACKET_SIZE(m_recv_buffer.data());
        uint32 total_size = size_field + PACKET_SIZE_FIELD;

        if (size_field < PACKET_OPCODE_SIZE) {
            disconnect();
            return false;
        }

        if (total_size > PACKET_SIZE_MAX) {
            disconnect();
            return false;
        }

        if (m_recv_buffer.size() < total_size)
            break;

        handle_packet(m_recv_buffer.data(), total_size);
        processed = true;

        m_recv_buffer.erase(m_recv_buffer.begin(), m_recv_buffer.begin() + total_size);
    }

    return processed;
}

void Session::handle_packet(const uint8* data, size_t len)
{
    packetdb().dispatch(this, data, len);
}

bool Session::send_packet(PacketBuffer& packet)
{
    if (!packet.is_finalized())
        packet.finalize();
    return send_raw(packet.data(), packet.size());
}

bool Session::send_raw(const uint8* data, size_t len)
{
    if (!is_connected() || data == nullptr || len == 0)
        return false;

    int sent = m_socket.send(data, len);
  
    if (sent != static_cast<int>(len))
        return false;

    m_last_send = gettick();
    return true;
}