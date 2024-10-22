#pragma once

#include "ozz/types.hpp"


namespace OZZ_NET
{
constexpr int socket_t_invalid{ -1 };
class sock_t
{
public:
    sock_t() noexcept = default;

    virtual ~sock_t()
    { close( sockfd ); }

    [[nodiscard]] bool create_socket(int socket_type) noexcept
    {
        if( ( sockfd = socket( AF_INET, socket_type, 0 ) ) == socket_t_invalid )
            return false;
        return true;
    }

    [[nodiscard]] bool create_socket(int socket_type, int socket_protocol) noexcept
    {
        if((sockfd = socket( AF_INET, socket_type, socket_protocol)) == socket_t_invalid)
            return false;
        return true;
    }

    inline const SOCKET_T& sock() noexcept
    { return (sockfd); }
    
protected:
    SOCKET_T sockfd{socket_t_invalid};
};

}
