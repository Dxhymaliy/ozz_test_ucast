
#include "ozz/net/unicast/udpserver.hpp"


namespace OZZ_NET::unicast{

udpserver::udpserver(const std::string ip, ushort port) noexcept
        : ucast(ip, port)
{}

bool udpserver::init() noexcept
{    
    if(!ucast::init())
    {
        LOG_ERROR("ucast::init()");
        return false;
    }

    if(!set_nonblock())
    {
        LOG_ERROR("set_nonblock");
        return false;
    }

    if(!bind())
    {
        LOG_ERROR("bind()");
        return false;
    }

    return true;
}

}