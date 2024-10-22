#include "ozz/utils/taskler.hpp"
#include <algorithm>

namespace OZZ_CORE::utils{
taskler::taskler(xpipe<request_msg_hdr>& customer, xpipe<dest_pack_data>& ring)
{
    auto num = std::thread::hardware_concurrency();
    size_t size = num > used_hardware ? num - used_hardware : min_work_threads;
    size = size > 1 ? size : 1;
    LOG_INFO("hardware_concurrency = %d size = %d", num, size);
    for(size_t i = 0; i < size; ++i)
    {
        generators.push_back(std::make_unique<generator>(customer, ring));
        generators.back()->start();
    }
}

taskler::~taskler()
{ stop(); }

void taskler::stop()
{
    if(!destroyed.exchange(true))
        for(decltype(auto) it : generators)
            it->stop();
}
}