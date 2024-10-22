#pragma once

#include "ozz/types.hpp"
#include "version.hpp"


namespace OZZ_CORE::protocol{

#pragma pack(push,1)
struct pack_hdr
{
    pack_hdr() noexcept = default;

    pack_hdr(msg_type type, uint32_t size) noexcept
        : size{size}, type{type}
    {}

    pack_hdr(const pack_hdr& ref) noexcept
    {
        *const_cast<float*>(&version) = ref.version;
        type = ref.type;
        size = ref.size;
        hdr_size = ref.hdr_size;
    }

    pack_hdr& operator=(const pack_hdr& ref) noexcept
    {
        *this = ref;
        return *this;
    }

    uint32_t    size{0};
    uint32_t    hdr_size{0};
    const float version{PROTOCOL_VERSION};
    msg_type    type{msg_type::undef};
    msg_state   state{msg_state::undef};
};
#pragma pack(pop)

constexpr uint32_t pack_hdr_size{sizeof(pack_hdr)};
}