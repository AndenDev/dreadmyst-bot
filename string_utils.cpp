#include "string_utils.h"
#include <string.h>
#include <stdlib.h>

namespace utils {

	void init_game_string(game_string* str, const char* text) {
		memset(str, 0, sizeof(game_string));
		size_t len = strlen(text);

		if (len < 16) {
			strcpy_s(str->m_buffer, 16, text);
			str->m_length = len;
			str->m_capacity = 15;
		}
		else {
			str->m_ptr = (char*)malloc(len + 1);
			strcpy_s(str->m_ptr, len + 1, text);
			str->m_length = len;
			str->m_capacity = len;
		}
	}

	void free_game_string(game_string* str) {
		if (str->m_capacity > 15 && str->m_ptr) {
			free(str->m_ptr);
		}
		memset(str, 0, sizeof(game_string));
	}

	void init_game_wstring(game_wstring* str, const WCHAR* text) {
		memset(str, 0, sizeof(game_wstring));
		size_t len = wcslen(text);

		if (len < 8) {
			wcscpy_s(str->m_buffer, 8, text);
			str->m_length = len;
			str->m_capacity = 7;
		}
		else {
			str->m_ptr = (WCHAR*)malloc((len + 1) * sizeof(WCHAR));
			wcscpy_s(str->m_ptr, len + 1, text);
			str->m_length = len;
			str->m_capacity = len;
		}
	}

	void free_game_wstring(game_wstring* str) {
		if (str->m_capacity > 7 && str->m_ptr) {
			free(str->m_ptr);
		}
		memset(str, 0, sizeof(game_wstring));
	}

} 