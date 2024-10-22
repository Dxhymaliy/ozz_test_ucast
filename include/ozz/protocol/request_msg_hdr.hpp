
#pragma once

#include <atomic>

#include "dest_pack_data.hpp"

namespace OZZ_CORE::protocol{
#pragma pack(push,4)
struct request_msg_hdr:public dest_msg_hdr
{
    request_msg_hdr() noexcept = default;

    request_msg_hdr(const struct sockaddr_in& addr, const msg_hdr& hdr) noexcept
        : dest_msg_hdr{addr, hdr}
    {}

    bool check_used() noexcept
    { return is_busy.exchange(true, std::memory_order_acquire); }

private:
    alignas(32) std::atomic_bool is_busy{false};
};

constexpr uint32_t  request_msg_hdr_size{sizeof(request_msg_hdr)};
}