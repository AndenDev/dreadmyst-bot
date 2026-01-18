#ifndef DREADMYST_COMMON_H
#define DREADMYST_COMMON_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <winhttp.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")


struct game_string {
	union {
		char m_buffer[16];
		char* m_ptr;
	};
	size_t m_length;
	size_t m_capacity;
};

struct game_wstring {
	union {
		WCHAR m_buffer[8];
		WCHAR* m_ptr;
	};
	size_t m_length;
	size_t m_capacity;
};

struct packet_buffer {
	void* m_vtable;
	void* m_unknown1;
	char* m_data_start;
	char* m_data_end;
};


typedef void(__thiscall* pfn_game_send_packet)(void*, void*);
typedef char(__stdcall* pfn_game_login)(
	WCHAR* server_url,
	DWORD endpoint,
	void* credentials,
	void* unused,
	void* out_token,
	void* out_error
	);
typedef int* (__cdecl* pfn_get_network_manager)();

#endif 