#pragma once

#include "ozz/net/unicast/udpserver.hpp"
#include "ozz/utils/taskler.hpp"

namespace OZZ_NET::unicast{

constexpr int socket_error_res{-1};
const auto desc_invalid_protocol_version{"Error: the version of protocol is too old."};
const auto desc_invalid_request{"Error: the requested data is invalid."};

using namespace utils;
using namespace protocol;
class server : public udpserver
{
public:
    /**
      * Constructor
      * @param conf json string
      */
    explicit server(const std::string& conf) noexcept;

    ~server();

    void run();

    void stop();

private:

    int send_invalid_version_error(struct sockaddr_in& from) noexcept;

    int send_invalid_request_error(struct sockaddr_in& from) noexcept;

    int get_and_send_generated_data() noexcept;

    void request_generated_data(struct sockaddr_in& from,  msg_hdr& hdr) noexcept;

private:
    xpipe<request_msg_hdr>          customer;
    xpipe<dest_pack_data>           ring;
    xpipe<dest_pack_data>::receiver channel;
    taskler                         generators;
    std::atomic_bool                running{false};
};

}
