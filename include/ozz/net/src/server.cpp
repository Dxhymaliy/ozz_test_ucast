
#include "ozz/net/server.hpp"

namespace OZZ_NET::unicast{

uint32_t number_of_node_to_request_for_generated_data{300};
uint32_t number_of_ready_data_to_transfer{1'000'000};
server::server(const std::string& conf) noexcept
        : udpserver(get_ip_from_json_str(conf), get_port_from_json_str(conf))
        , customer(number_of_node_to_request_for_generated_data)
        , ring(number_of_ready_data_to_transfer)
        , channel(ring)
        , generators(customer, ring)
{}

server::~server()
{ stop(); }

void server::run()
{
    running.exchange(true);
    char data[max_mtu];

    while(running)
    {
        struct sockaddr_in from;
        memset(&from, 0, sizeof(from)); 
        int res = recv(from, data, max_mtu);


        if(res != socket_error_res && res >= msg_hdr_size)
        {
            msg_hdr* hdr = (msg_hdr*)data;
            if(hdr->version < PROTOCOL_VERSION)
            {
                send_invalid_version_error(from);
            }
            else if(hdr->type == msg_type::auth)
            {
                msg_hdr auth_hdr;
                auth_hdr.hdr_size = protocol::msg_hdr_size;
                auth_hdr.seq = 1;
                auth_hdr.type = msg_type::confirm;
                auth_hdr.state = msg_state::fin;
                auth_hdr.num_pck = 1;
                auth_hdr.size = protocol::msg_hdr_size;
                auth_hdr.req_num = hdr->req_num;
                send(from, (char*)&(auth_hdr), auth_hdr.size);
            }
            else if(!hdr->req_num)
            {
                send_invalid_request_error(from);
            }
            else
            {
                request_generated_data(from, *hdr);
            }
        }
        get_and_send_generated_data();
    }
}

void server::stop()
{
    if(running.exchange(false))
        generators.stop();
}

int server::send_invalid_version_error(struct sockaddr_in& from) noexcept
{
    const size_t sz{pack_hdr_size + strlen(desc_invalid_protocol_version) + 1};
    char data[sz];
    pack_hdr* hdr = (pack_hdr*)data;
    hdr->hdr_size = pack_hdr_size;
    hdr->size = sz;
    hdr->type = msg_type::error_msg;
    hdr->state = msg_state::fin;

    memcpy(data + pack_hdr_size, desc_invalid_protocol_version, strlen(desc_invalid_protocol_version));

    return send(from, data, sz);
}

int server::send_invalid_request_error(struct sockaddr_in& from) noexcept
{
    const size_t sz{pack_hdr_size + strlen(desc_invalid_request) + 1};
    char data[sz];
    pack_hdr* hdr = (pack_hdr*)data;
    hdr->hdr_size = pack_hdr_size;
    hdr->size = sz;
    hdr->type = msg_type::error_msg;
    hdr->state = msg_state::fin;

    memcpy(data + pack_hdr_size, desc_invalid_protocol_version, strlen(desc_invalid_request));

    return send(from, data, sz);
}

void server::request_generated_data(struct sockaddr_in& from,  msg_hdr& hdr) noexcept
{
    request_msg_hdr request;
    memcpy(&request.dest_addr, &from, sizeof(sockaddr_in));   
    memcpy(&request.data_hdr, &hdr, sizeof(msg_hdr));
    customer.push(request);
    customer.notify_all();
}

int server::get_and_send_generated_data() noexcept
{
    dest_pack_data* ptr = channel.recv();
    if(nullptr == ptr)
        return 0;

    int res = send(ptr->dest_addr, (char*)&(ptr->data_hdr), ptr->data_hdr.size);
    if(res == -1)
        res = send(ptr->dest_addr, (char*)&(ptr->data_hdr), ptr->data_hdr.size);
    return res;
}

}