#ifndef SOCKET_HPP
#define SOCKET_HPP

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include "../core/cbasetypes.hpp"
#include <string>

#ifdef PLATFORM_WINDOWS
using socket_t = SOCKET;
constexpr socket_t INVALID_SOCKET_VAL = INVALID_SOCKET;
#define CLOSESOCKET(s) closesocket(s)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
using socket_t = int;
constexpr socket_t INVALID_SOCKET_VAL = -1;
#define CLOSESOCKET(s) ::close(s)
#endif

bool socket_init(void);
void socket_final(void);

class Socket {
public:
    Socket();
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    bool create(void);
    void close(void);
    bool is_valid(void) const { return m_fd != INVALID_SOCKET_VAL; }
    bool connect(const std::string& host, uint16 port);
    bool set_nodelay(bool nodelay);
    bool set_recv_timeout(uint32 timeout_ms);
    int send(const uint8* data, size_t len);
    int recv(uint8* buffer, size_t len);
    bool poll_read(uint32 timeout_ms);

private:
    socket_t m_fd;
};

#endif