#ifndef SESSION_HPP
#define SESSION_HPP

#include "../core/cbasetypes.hpp"
#include "../core/timer.hpp"
#include "socket.hpp"
#include "../packet/packet_base.hpp"
#include <vector>
#include <string>

enum SessionState : uint8 {
    SESSION_STATE_DISCONNECTED = 0,
    SESSION_STATE_CONNECTING,
    SESSION_STATE_CONNECTED,
    SESSION_STATE_AUTHENTICATING,
    SESSION_STATE_AUTHENTICATED,
    SESSION_STATE_CHARSELECT,
    SESSION_STATE_ENTERING_WORLD,
    SESSION_STATE_INGAME,
    SESSION_STATE_DISCONNECTING,
};

struct CharacterInfo {
    uint32      guid;
    std::string name;
    uint8       class_id;
    uint8       gender;
    int32       level;

    CharacterInfo() : guid(0), class_id(0), gender(0), level(1) {}
};

class Session {
public:
    Session();
    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    bool connect(const std::string& host, uint16 port);
    void disconnect(void);
    bool is_connected(void) const { return m_state >= SESSION_STATE_CONNECTED; }

    SessionState state(void) const { return m_state; }
    void set_state(SessionState state) { m_state = state; }

    bool process(void);

    bool send_packet(PacketBuffer& packet);
    bool send_raw(const uint8* data, size_t len);

    void set_authenticated(bool auth) { m_authenticated = auth; }
    bool is_authenticated(void) const { return m_authenticated; }

    void set_server_time(int64 time) { m_server_time = time; }
    int64 server_time(void) const { return m_server_time; }

    const std::vector<CharacterInfo>& characters(void) const { return m_characters; }
    std::vector<CharacterInfo>& characters(void) { return m_characters; }
    void clear_characters(void) { m_characters.clear(); }

    const CharacterInfo* get_character_by_name(const std::string& name) const {
        for (const auto& ch : m_characters) {
            if (ch.name == name)
                return &ch;
        }
        return nullptr;
    }

    void set_in_world(bool in_world) { m_in_world = in_world; }
    bool is_in_world(void) const { return m_in_world; }

    void set_map_id(int32 id) { m_map_id = id; }
    int32 map_id(void) const { return m_map_id; }

    void set_controlled_guid(uint32 guid) { m_controlled_guid = guid; }
    uint32 controlled_guid(void) const { return m_controlled_guid; }

    void set_position(float x, float y) { m_x = x; m_y = y; }
    float x(void) const { return m_x; }
    float y(void) const { return m_y; }

private:
    Socket        m_socket;
    SessionState  m_state;
    std::string   m_host;
    uint16        m_port;

    std::vector<uint8> m_recv_buffer;
    static const size_t RECV_BUFFER_SIZE = 65536;

    bool   m_authenticated;
    int64  m_server_time;

    std::vector<CharacterInfo> m_characters;

    bool   m_in_world;
    int32  m_map_id;
    uint32 m_controlled_guid;
    float  m_x;
    float  m_y;

    t_tick m_last_recv;
    t_tick m_last_send;

    bool process_recv(void);
    void handle_packet(const uint8* data, size_t len);
};

#endif