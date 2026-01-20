#ifndef DREADMYST_STRING_UTILS_H
#define DREADMYST_STRING_UTILS_H

#include "common.h"

namespace utils {

	void init_game_string(game_string* str, const char* text);
	void free_game_string(game_string* str);

	void init_game_wstring(game_wstring* str, const WCHAR* text);
	void free_game_wstring(game_wstring* str);

}

#endif 