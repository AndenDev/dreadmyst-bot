/**
 * @file connector.cpp
 * @brief HTTPS authentication connector implementation
 */
#include "connector.hpp"
#include "../core/showmsg.hpp"

#ifdef PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

 // For JSON parsing - you can use nlohmann/json or a simple parser
 // For now, we'll do basic string parsing for the token

#include <sstream>
#include <vector>

// =============================================================================
// Base64 Encoding
// =============================================================================

static const char base64_chars[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string Connector::base64_encode(const std::string& input)
{
    std::string output;
    output.reserve(((input.size() + 2) / 3) * 4);

    uint32 val = 0;
    int valb = -6;

    for (uint8 c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (output.size() % 4) {
        output.push_back('=');
    }

    return output;
}

// =============================================================================
// Simple JSON token extraction
// =============================================================================

static bool extract_json_string(const std::string& json, const std::string& key, std::string& value)
{
    // Look for "key":"value" or "key": "value"
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos)
        return false;

    pos += search.length();

    // Skip whitespace and colon
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == ':'))
        pos++;

    if (pos >= json.size() || json[pos] != '"')
        return false;

    pos++; // Skip opening quote

    // Find closing quote
    size_t end = pos;
    while (end < json.size() && json[end] != '"') {
        if (json[end] == '\\' && end + 1 < json.size())
            end++; // Skip escaped char
        end++;
    }

    value = json.substr(pos, end - pos);
    return true;
}

static bool extract_json_bool(const std::string& json, const std::string& key, bool& value)
{
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos)
        return false;

    pos += search.length();

    // Skip whitespace and colon
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == ':'))
        pos++;

    if (json.substr(pos, 4) == "true") {
        value = true;
        return true;
    }
    if (json.substr(pos, 5) == "false") {
        value = false;
        return true;
    }

    return false;
}

// =============================================================================
// Connector Implementation
// =============================================================================

Connector::Connector()
{
}

Connector::~Connector()
{
}

#ifdef PLATFORM_WINDOWS

bool Connector::authenticate(const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password,
    std::string& out_token,
    std::string& out_error)
{
    out_token.clear();
    out_error.clear();


    std::string credentials = username + ":" + password;
    std::string payload = base64_encode(credentials);

    ShowInfo("Authenticating via HTTPS to %s%s...\n", host.c_str(), path.c_str());
    
    std::wstring whost(host.begin(), host.end());
    std::wstring wpath(path.begin(), path.end());

    
    HINTERNET hSession = WinHttpOpen(L"GameClient/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession) {
        out_error = "Failed to initialize HTTP connection";
        ShowError("WinHttpOpen failed: %lu\n", GetLastError());
        return false;
    }

   
    DWORD timeout = 30000;
    WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hSession, WINHTTP_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

   
    HINTERNET hConnect = WinHttpConnect(hSession, whost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        DWORD err = GetLastError();
        WinHttpCloseHandle(hSession);

        if (err == ERROR_WINHTTP_NAME_NOT_RESOLVED)
            out_error = "Could not reach server. Check your internet connection.";
        else
            out_error = "Failed to connect to auth server";

        ShowError("WinHttpConnect failed: %lu\n", err);
        return false;
    }

   
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wpath.c_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        out_error = "Failed to create HTTP request";
        ShowError("WinHttpOpenRequest failed: %lu\n", GetLastError());
        return false;
    }

   
    LPCWSTR headers = L"User-Agent: GameClient/1.0\r\nContent-Type: text/plain";

  
    if (!WinHttpSendRequest(hRequest, headers, (DWORD)-1L,
        (LPVOID)payload.c_str(), (DWORD)payload.size(),
        (DWORD)payload.size(), 0)) {
        DWORD err = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        switch (err) {
        case ERROR_WINHTTP_TIMEOUT:
            out_error = "Connection timed out. Please try again.";
            break;
        case ERROR_WINHTTP_CANNOT_CONNECT:
            out_error = "Could not connect to server. It may be down.";
            break;
        case ERROR_WINHTTP_SECURE_FAILURE:
            out_error = "Secure connection failed. Check your system time.";
            break;
        default:
            out_error = "Failed to send HTTP request";
        }

        ShowError("WinHttpSendRequest failed: %lu\n", err);
        return false;
    }

   
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        DWORD err = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        if (err == ERROR_WINHTTP_TIMEOUT)
            out_error = "Server took too long to respond.";
        else
            out_error = "Failed to receive response from server";

        ShowError("WinHttpReceiveResponse failed: %lu\n", err);
        return false;
    }

   
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);

    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response.append(buffer, bytesRead);
        bytesRead = 0;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);


    
    bool success = false;
    extract_json_bool(response, "success", success);

    if (statusCode == 200 && success) {
        if (extract_json_string(response, "token", out_token)) {
            m_token = out_token;
            ShowStatus("Authentication successful! Token received.\n");
            return true;
        }
        out_error = "Invalid response from server (no token)";
        return false;
    }

    
    if (!extract_json_string(response, "error", out_error)) {
        switch (statusCode) {
        case 401:
            out_error = "Invalid username or password";
            break;
        case 403:
            out_error = "Access denied";
            break;
        case 429:
            out_error = "Too many attempts. Please wait.";
            break;
        case 500:
        case 502:
        case 503:
            out_error = "Server temporarily unavailable";
            break;
        default:
            out_error = "Authentication failed (HTTP " + std::to_string(statusCode) + ")";
        }
    }

    ShowError("Authentication failed: %s\n", out_error.c_str());
    return false;
}

#else

bool Connector::authenticate(const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password,
    std::string& out_token,
    std::string& out_error)
{
    out_error = "HTTPS auth not implemented for this platform";
    ShowError("%s\n", out_error.c_str());
    return false;
}
#endif