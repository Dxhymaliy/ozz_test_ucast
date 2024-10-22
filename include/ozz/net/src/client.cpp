#pragma once

#include "ozz/net/client.hpp"


namespace OZZ_NET::unicast{

using namespace std::chrono_literals;

client::client(const std::string& conf)  noexcept
        : ucast(get_ip_from_json_str(conf), get_port_from_json_str(conf))
        , request_number(get_number_from_json_str(conf, "request_number"))
{
    send_request_number_hdr.hdr_size = protocol::msg_hdr_size;
    send_request_number_hdr.seq = 1;
    send_request_number_hdr.type = msg_type::ask_msg;
    send_request_number_hdr.state = msg_state::fin;
    send_request_number_hdr.num_pck = 1;
    send_request_number_hdr.size = protocol::msg_hdr_size; 
    send_request_number_hdr.req_num = request_number;

    auth_hdr.hdr_size = protocol::msg_hdr_size;
    auth_hdr.seq = 1;
    auth_hdr.type = msg_type::auth;
    auth_hdr.state = msg_state::fin;
    auth_hdr.num_pck = 1;
    auth_hdr.size = protocol::msg_hdr_size;
    auth_hdr.req_num = request_number;

    arr.reserve(gen_numbers);
    init_delimeter(request_number, delimeter);
}

client::~client() noexcept
{ stop(); }

bool client::init() noexcept
{
    if(!ucast::init())
        return false;

    if(!set_nonblock())
    {
        LOG_ERROR("set_nonblock");
        return false;
    }

    return true;
}

bool client::authority() noexcept
{
    while(attempts < max_attempts)
    {
        send((char*)&auth_hdr, protocol::msg_hdr_size);

        int res = recv((char*)buf, max_mtu);
        if(res == socket_error_res)
        {
            std::this_thread::sleep_for(1ms);
            ++attempts;
            continue;
        }

        if(res >= pack_hdr_size && ((msg_hdr*)buf)->type == msg_type::confirm)
        {
                std::this_thread::sleep_for(3000ms);
                break;
        }
    }

    return attempts < max_attempts;
}

void client::run()
{
    if(!authority())
    {
        LOG_ERROR("Cannot authoritazed");
        return;
    }

    if(!send_number_request())
    {
        LOG_ERROR("Cannot send number request");
        return;
    }

    running = true;
    attempts = 0;

    while(running)
    {
        if(!read_data())
            break;
    }

    std::sort(arr.begin(), arr.end(), [](double a, double b){return a > b; });

    write_file();
}

void client::stop() noexcept
{ running.exchange(false); }

bool client::send_number_request() noexcept
{
    return send((char*)&send_request_number_hdr, protocol::msg_hdr_size) != socket_error_res;
}

bool client::handle_response(pack_hdr& hdr, int res)
{
    if(hdr.type == msg_type::answer_msg && res == hdr.size && res >= msg_hdr_size)
    {    
        msg_hdr msg = *(msg_hdr*)buf;
        if(!num_packets)
        {
            num_packets = msg.num_pck;
            ++count;
        }
        else
        {
            if(msg.seq != count)//@warning That need to use for request to server for recovery
            {
                LOG_WARNING("missed packet num_packets{%llu} != msg.num_pck{%llu} + count{%llu}/seq{%llu} = %llu"
                        , num_packets, msg.num_pck, count, msg.seq, msg.num_pck + count);
            }

            ++count;

            if(uint32_t sz{(msg.size - msg.hdr_size)/sizeof(int64_t)}; sz > 0)
            {
                int64_t* ptr = (int64_t*)(buf + msg_hdr_size);
                for(size_t i = 0; i < sz; ++i)
                {
                    int64_t val = (int64_t)*(ptr + i);
                    arr.push_back(static_cast<double>(val)/delimeter);
                }
            }

            if(count == num_packets || hdr.state == msg_state::fin)
                    return false;
        }
    }

    return true;
}

bool client::read_data()
{
    if(attempts > max_attempts)
    {
        LOG_ERROR("Cannot read from socket");
        return false;
    }
    bool ret{true};
    int res = recv((char*)buf, max_mtu);

    if(res == socket_error_res)   
    {
        std::this_thread::sleep_for(1ms);
        ++attempts;
        return true;
    }
        
    attempts = 0;
    if(res >= pack_hdr_size)
    {
        pack_hdr hdr = *(pack_hdr*)buf;
        ret = handle_response(hdr, res);
    }else
    {
        LOG_ERROR("Invalid data from server res = %d", res);
        ret = false;
    }
    return ret;
}

}