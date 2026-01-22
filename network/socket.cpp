#include "socket.hpp"
#include <cstring>

#ifdef PLATFORM_WINDOWS
static bool g_wsa_initialized = false;
#endif

bool socket_init(void)
{
#ifdef PLATFORM_WINDOWS
    if (!g_wsa_initialized) {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            return false;
        g_wsa_initialized = true;
    }
#endif
    return true;
}

void socket_final(void)
{
#ifdef PLATFORM_WINDOWS
    if (g_wsa_initialized) {
        WSACleanup();
        g_wsa_initialized = false;
    }
#endif
}

Socket::Socket()
    : m_fd(INVALID_SOCKET_VAL)
{
}

Socket::~Socket()
{
    close();
}

bool Socket::create(void)
{
    if (is_valid())
        return true;

    m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return m_fd != INVALID_SOCKET_VAL;
}

void Socket::close(void)
{
    if (is_valid()) {
        CLOSESOCKET(m_fd);
        m_fd = INVALID_SOCKET_VAL;
    }
}

bool Socket::connect(const std::string& host, uint16 port)
{
    if (!is_valid())
        return false;

    struct addrinfo hints = {};
    struct addrinfo* result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host.c_str(), port_str, &hints, &result) != 0)
        return false;

    bool connected = false;
    for (struct addrinfo* addr = result; addr != nullptr; addr = addr->ai_next) {
        if (::connect(m_fd, addr->ai_addr, (int)addr->ai_addrlen) == 0) {
            connected = true;
            break;
        }
    }

    freeaddrinfo(result);
    return connected;
}

bool Socket::set_nodelay(bool nodelay)
{
    if (!is_valid())
        return false;
    int flag = nodelay ? 1 : 0;
    return setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag)) == 0;
}

bool Socket::set_recv_timeout(uint32 timeout_ms)
{
    if (!is_valid())
        return false;
#ifdef PLATFORM_WINDOWS
    DWORD tv = timeout_ms;
    return setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0;
#else
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    return setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0;
#endif
}

int Socket::send(const uint8* data, size_t len)
{
    if (!is_valid() || data == nullptr || len == 0)
        return -1;
    return ::send(m_fd, (const char*)data, (int)len, 0);
}

int Socket::recv(uint8* buffer, size_t len)
{
    if (!is_valid() || buffer == nullptr || len == 0)
        return -1;
    return ::recv(m_fd, (char*)buffer, (int)len, 0);
}

bool Socket::poll_read(uint32 timeout_ms)
{
    if (!is_valid())
        return false;

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_fd, &read_fds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select((int)m_fd + 1, &read_fds, nullptr, nullptr, &tv);
    return ret > 0 && FD_ISSET(m_fd, &read_fds);
}