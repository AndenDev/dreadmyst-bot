#ifndef DREADMYST_PACKETS_H
#define DREADMYST_PACKETS_H

#include "common.h"

namespace network {

	enum packet_id : WORD {
		PACKET_AUTH = 0x0002,
		PACKET_CHARACTER_SELECT = 0x0005,
	};

#pragma pack(push, 1)

	struct auth_packet {
		WORD packet_id;          
		char token[256];         
		DWORD build_version;    
		char hardware_id[33];    
	};

	struct character_select_packet {
		WORD packet_id;          
		DWORD character_id;      
	};

#pragma pack(pop)

}

#endif
