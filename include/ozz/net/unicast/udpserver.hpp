
#pragma once

#include "ucast.hpp"


namespace OZZ_NET::unicast{

class udpserver : public ucast, noncopyable
{
public:
    /**
      * Constructor
      * @param ip UDP server address
      * @param port UDP server port
      */
    udpserver(const std::string ip, ushort port) noexcept;

    ~udpserver() noexcept = default;

    bool init() noexcept;
};

}
