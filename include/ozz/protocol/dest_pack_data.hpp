
#pragma once

#include "msg_hdr.hpp"


namespace OZZ_CORE::protocol{

constexpr uint32_t  send_data_size{max_mtu - msg_hdr_size};
constexpr uint32_t number_val_in_pack{protocol::send_data_size/sizeof(int64_t)};

#pragma pack(push,1)
struct resp_msg_data : public msg_hdr
{
    resp_msg_data() noexcept = default;

    resp_msg_data(const msg_hdr& hdr) noexcept
        : msg_hdr{hdr}
    {
        memset(data, 0, number_val_in_pack * sizeof(int64_t));
    }

    int64_t data[number_val_in_pack];
};
#pragma pack(pop)

constexpr uint32_t resp_msg_data_size{sizeof(resp_msg_data)};

#pragma pack(push,1)
struct dest_msg_hdr
{
    dest_msg_hdr() noexcept = default;

    dest_msg_hdr(const struct sockaddr_in& addr, const msg_hdr& hdr) noexcept
        : dest_addr{addr}, data_hdr{hdr}
    {}

    sockaddr_in dest_addr;
    msg_hdr data_hdr;
};
#pragma pack(pop)

constexpr uint32_t  dest_msg_hdr_size{sizeof(dest_msg_hdr)};

#pragma pack(push,1)

struct dest_pack_data
{
    dest_pack_data() noexcept = default;

    dest_pack_data(const struct dest_msg_hdr& hdr) noexcept
        : dest_addr{hdr.dest_addr}, data_hdr{hdr.data_hdr}
    {}

    dest_pack_data(const struct sockaddr_in& addr, const msg_hdr& hdr) noexcept
        : dest_addr{addr}, data_hdr{hdr}
    {}

    sockaddr_in  dest_addr;
    resp_msg_data data_hdr;
};
#pragma pack(pop)
constexpr uint32_t dest_pack_data_size{sizeof(dest_pack_data)};

}