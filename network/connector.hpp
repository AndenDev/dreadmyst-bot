
#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include "../core/cbasetypes.hpp"
#include <string>

class Connector {
public:
    Connector();
    ~Connector();


    bool authenticate(const std::string& host,
        const std::string& path,
        const std::string& username,
        const std::string& password,
        std::string& out_token,
        std::string& out_error);


    const std::string& token() const { return m_token; }
    bool has_token() const { return !m_token.empty(); }

private:
    std::string m_token;
    static std::string base64_encode(const std::string& input);
};

#endif // CONNECTOR_HPP