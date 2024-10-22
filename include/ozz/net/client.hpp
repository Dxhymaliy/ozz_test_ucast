#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

#include "ozz/net/unicast/ucast.hpp"
#include "ozz/protocol/dest_pack_data.hpp"


namespace OZZ_NET::unicast{

constexpr int socket_error_res{-1};

using namespace protocol;
constexpr int max_attempts{1000};
class client : public ucast
{
public:
    /**
      * Constructor
      * @param conf json string
      */
    explicit client(const std::string& conf)  noexcept;

    ~client() noexcept;

    bool init() noexcept;

    void run();

    void stop() noexcept;

private:

    bool authority() noexcept;

    bool send_number_request() noexcept;

    bool handle_response(pack_hdr& hdr, int res);

    bool read_data();

    bool write_file()
    { 
        char data[256];
        sprintf(data, "%u_%u_unique_double", time(nullptr), arr.size());
        std::fstream binary_file(data, std::ios::out | std::ios::binary | std::ios::app);
        if(!binary_file)
        {
            LOG_ERROR("Cannot open file '%s'", data);
            return false;
        }

        for(auto d : arr)
            binary_file.write((char*)(&d),sizeof(double));

        return true;
    }

private:
    uint64_t            request_number;
    uint64_t            count{0};
    int                 attempts{0};
    uint64_t            num_packets{0};
    uint                delimeter;
    msg_hdr             send_request_number_hdr;
    msg_hdr             auth_hdr;
    std::atomic_bool    running{false};
    std::vector<double> arr;
    char                buf[max_mtu];
};

}