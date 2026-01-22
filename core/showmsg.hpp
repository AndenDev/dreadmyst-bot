#ifndef SHOWMSG_HPP
#define SHOWMSG_HPP

#include "cbasetypes.hpp"

#define CL_RESET      "\033[0m"
#define CL_CLS        "\033[2J"
#define CL_CLL        "\033[K"


#define CL_BOLD       "\033[1m"
#define CL_NORM       CL_RESET
#define CL_NORMAL     CL_RESET
#define CL_NONE       CL_RESET


#define CL_BLACK      "\033[30m"
#define CL_RED        "\033[31m"
#define CL_GREEN      "\033[32m"
#define CL_YELLOW     "\033[33m"
#define CL_BLUE       "\033[34m"
#define CL_MAGENTA    "\033[35m"
#define CL_CYAN       "\033[36m"
#define CL_WHITE      "\033[37m"
#define CL_GRAY       "\033[90m"
#define CL_GREY       CL_GRAY


#define CL_BT_BLACK   "\033[90m"
#define CL_BT_RED     "\033[91m"
#define CL_BT_GREEN   "\033[92m"
#define CL_BT_YELLOW  "\033[93m"
#define CL_BT_BLUE    "\033[94m"
#define CL_BT_MAGENTA "\033[95m"
#define CL_BT_CYAN    "\033[96m"
#define CL_BT_WHITE   "\033[97m"


#define CL_BG_BLACK   "\033[40m"
#define CL_BG_RED     "\033[41m"
#define CL_BG_GREEN   "\033[42m"
#define CL_BG_YELLOW  "\033[43m"
#define CL_BG_BLUE    "\033[44m"
#define CL_BG_MAGENTA "\033[45m"
#define CL_BG_CYAN    "\033[46m"
#define CL_BG_WHITE   "\033[47m"


#define CL_WTBL       "\033[37;44m"   
#define CL_XXBL       "\033[0;44m"    
#define CL_PASS       "\033[0;32;42m" 

void showmsg_init(bool use_colors = true);
void showmsg_final(void);

void ShowMessage(const char* format, ...);
void ShowStatus(const char* format, ...);
void ShowSQL(const char* format, ...);
void ShowInfo(const char* format, ...);
void ShowNotice(const char* format, ...);
void ShowWarning(const char* format, ...);
void ShowDebug(const char* format, ...);
void ShowError(const char* format, ...);
void ShowFatalError(const char* format, ...);

#endif