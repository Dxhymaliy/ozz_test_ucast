#include <fcntl.h>

#include "ozz/net/unicast/ucast.hpp"


namespace OZZ_NET::unicast{

ucast::ucast(const std::string& ip, ushort port) noexcept
    : ip{ip}, port{port}
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(inet_addr(ip.c_str()));//INADDR_ANY;
}

bool ucast::init() noexcept
{
    return create_socket(SOCK_DGRAM);
}

int ucast::connect()
{
    return ::connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
}

bool ucast::bind() noexcept
{
    return ::bind(sockfd, (const struct sockaddr *)/*reinterpret_cast<sockaddr*>*/(&addr), sizeof(addr)) == 0;
}

bool ucast::receive_timeout(uint milliseconds) noexcept
{
    struct timeval param;
    if(milliseconds)
    {
        param.tv_sec = time_t(milliseconds / milli_sec_per_sec);
        param.tv_usec = suseconds_t(milliseconds % milli_sec_per_sec);
    }else
    {
        param.tv_sec = 0;
        param.tv_usec = 1;
    }

    return ::setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &param, sizeof(param)) < 0;
}

bool ucast::set_nonblock() noexcept
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if(flags == -1)
    {
        LOG_ERROR("fcntl error1");
        return false;
    }
    flags |= O_NONBLOCK;
    if(-1 == fcntl(sockfd, F_SETFL, flags))
    {
        LOG_ERROR("fcntl error2");
        return false;
    }

    return true;
}

int ucast::recv(struct sockaddr_in& from, char* data, size_t size) noexcept
{
    socklen_t len;

    return ::recvfrom(sockfd, data, size
            , 0, reinterpret_cast<sockaddr*>(&from), &len);
}

int ucast::recv(char* data, size_t size) noexcept
{
    socklen_t len;

    return ::recvfrom(sockfd, data, size
            , 0, reinterpret_cast<sockaddr*>(&addr), &len);
}

int ucast::send(struct sockaddr_in& dest, char* data, size_t size) noexcept
{
    return sendto(sockfd, data, size, 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
}

int ucast::send(char* data, size_t size) noexcept
{
    return sendto(sockfd, data, size, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
}

}