#pragma once

#include "pack_hdr.hpp"


namespace OZZ_CORE::protocol{
#pragma pack(push,1)
struct msg_hdr : public pack_hdr
{
    msg_hdr() noexcept = default;

    msg_hdr(msg_type type, uint32_t size) noexcept
        : pack_hdr{type, size}
    {}

    uint32_t seq{0};// number of sent sequence
    uint32_t num_pck{0};// remaining number of packets 
    uint64_t req_num{0};// required number
};
#pragma pack(pop)

constexpr uint32_t  msg_hdr_size{sizeof(msg_hdr)};
}