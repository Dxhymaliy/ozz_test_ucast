
#pragma once

#include "ozz/net/socket.hpp"


namespace OZZ_NET::unicast{

class ucast : public sock_t
{
public:
    /**
      * Constructor
      * @param ip UDP server address
      * @param port UDP server port
      */
    ucast(const std::string& ip, ushort port) noexcept;

    ~ucast() noexcept = default;

    bool init() noexcept;

    // void init_addr() noexcept;

    // void init_addr_any() noexcept;

    int connect();

    /**
      * Receive of data.
      * \param[in,out] from Reference to remote address.
      * \param[in,out] data Reference to block memory.
      * \param[in] size Size of waiting data.
      * \return Number of received data or -1 if error.
      */
    int recv(struct sockaddr_in& from, char* data, size_t size) noexcept;

    /**
      * Receive of data.
      * \param[in,out] data Reference to block memory.
      * \param[in] size Size of waiting data.
      * \return Number of received data or -1 if error.
      */
    int recv(char* data, size_t size) noexcept;

    /**
      * Send data.
      * \param[in,out] to Reference to remote address.
      * \param[in,] data Send block memory.
      * \param[in] size Size of sending data.
      * \return Number of received data or -1 if error.
      */
    int send(struct sockaddr_in& dest, char* data, size_t size) noexcept;

    /**
      * Send data.
      * \param[in,] data Send block memory.
      * \param[in] size Size of sending data.
      * \return Number of received data or -1 if error.
      */
    int send(char* data, size_t size) noexcept;

    bool receive_timeout(uint milliseconds) noexcept;

    bool set_nonblock() noexcept;

    bool bind() noexcept;

private:
    std::string ip;
    ushort      port;
    struct sockaddr_in addr;
};


}
