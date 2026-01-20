#ifndef DREADMYST_AUTH_H
#define DREADMYST_AUTH_H

#include "common.h"
#include <string>

namespace network {

	enum packet_id : WORD {
   		 PACKET_AUTH = 0x0002,
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
	class auth {
	public:	
		static bool login(const std::string& username, const std::string& password);
		static const std::string& get_session_token();
		static const std::string& get_last_error();

	private:
		static std::string s_session_token;
		static std::string s_last_error;
	};
} 
#endif
