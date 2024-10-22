#pragma once

#include <vector>

#include "generator.hpp"


namespace OZZ_CORE::utils{
const uint used_hardware{3};
const uint min_work_threads{2};

    class taskler : public noncopyable
    {
    public:
        taskler(xpipe<request_msg_hdr>& customer, xpipe<dest_pack_data>& ring);
        
        ~taskler();

        void stop();

        private:
        std::vector<std::unique_ptr<generator>> generators;
        std::atomic_bool destroyed{false};
    };
}