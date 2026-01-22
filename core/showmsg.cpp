#include "showmsg.hpp"
#include <cstdio>
#include <cstdarg>
#include <ctime>

#ifdef PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

static bool g_use_colors = true;
static bool g_stdout_with_ansisequence = false;

static void print_timestamp(void)
{
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    printf("[%02d:%02d:%02d]", t->tm_hour, t->tm_min, t->tm_sec);
}

void showmsg_init(bool use_colors)
{
    g_use_colors = use_colors;
    g_stdout_with_ansisequence = false;

#ifdef PLATFORM_WINDOWS
    if (use_colors) {
   
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (SetConsoleMode(hOut, dwMode)) {
                    g_stdout_with_ansisequence = true;
                }
            }
        }
        if (!g_stdout_with_ansisequence) {
            g_use_colors = false;
        }
    }
#else
    g_stdout_with_ansisequence = use_colors;
#endif
}

void showmsg_final(void)
{
}

void ShowMessage(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowStatus(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_GREEN "[Status]" CL_RESET ": ");
    else
        printf("[Status]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowSQL(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_MAGENTA "[SQL]" CL_RESET ": ");
    else
        printf("[SQL]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowInfo(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_CYAN "[Info]" CL_RESET ": ");
    else
        printf("[Info]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowNotice(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_WHITE "[Notice]" CL_RESET ": ");
    else
        printf("[Notice]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowWarning(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_YELLOW "[Warning]" CL_RESET ": ");
    else
        printf("[Warning]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowDebug(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_GRAY "[Debug]" CL_RESET ": ");
    else
        printf("[Debug]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowError(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_RED "[Error]" CL_RESET ": ");
    else
        printf("[Error]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void ShowFatalError(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    print_timestamp();
    if (g_use_colors)
        printf(CL_RED CL_BOLD "[Fatal Error]" CL_RESET ": ");
    else
        printf("[Fatal Error]: ");
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}