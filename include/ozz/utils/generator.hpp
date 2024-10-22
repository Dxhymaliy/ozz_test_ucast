#pragma once

#include <thread>

#include "xpipe.hpp"
#include "unique_values.hpp"
#include "ozz/protocol/request_msg_hdr.hpp"

namespace OZZ_CORE::utils{

    using namespace OZZ_CORE::protocol;
    
    class generator
    {
    public:
        generator(xpipe<request_msg_hdr>& customer, xpipe<dest_pack_data>& ring) noexcept;
        
        ~generator();

        bool init() noexcept;

        void start()
        { job = std::thread(std::ref(*this)); }

        void generate_unique_value() noexcept;

        void send_data() noexcept
        {
            auto [ptr, seq] = ring.push(pack);
            ptr->data_hdr.seq = seq;
        }

        void update_hdr(size_t count) noexcept;
        
        void operator()() noexcept;

        void stop() noexcept;

        void clean_unique_numbers() noexcept
        { memset(pack.data_hdr.data, 0, number_val_in_pack * sizeof(double)); }

    private:

        xpipe<request_msg_hdr>::receiver receiver;
        xpipe<dest_pack_data>&           ring;
        std::random_device               os_seed;
        engine                           gen;
        dest_pack_data                   pack;
        unique_values                    unique_val;
        std::thread                      job;
        std::atomic_bool                 running{true};
    };
}