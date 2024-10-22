
#include "ozz/utils/generator.hpp"

namespace OZZ_CORE::utils{
using namespace OZZ_CORE::protocol;

generator::generator(xpipe<request_msg_hdr>& customer, xpipe<dest_pack_data>& ring) noexcept
        : receiver{customer}
        , ring{ring}
        , gen(os_seed())
{}

generator::~generator()
{ stop(); }

bool generator::init() noexcept
{
    receiver.wait();

    bool res{true};
    for(;;)
    {
        request_msg_hdr* ptr = receiver.recv();
        if(nullptr == ptr)
        {
            res = false;
            break;
        }
        
        if(!ptr->check_used())
        {
            memcpy(&pack.dest_addr, &ptr->dest_addr, sizeof(sockaddr_in));
            memcpy(&pack.data_hdr, &ptr->data_hdr, sizeof(msg_hdr));
            break;
        }  
    }

    return res;
}

void generator::generate_unique_value() noexcept
{
    engine gen(os_seed());

    init_delimeter(pack.data_hdr.req_num, unique_val.delimeter);
    unique_val.rebase();

    long long l = pack.data_hdr.req_num * 2;
    std::uniform_int_distribution< u32 > distribute(0 , l);
    size_t count{0};
    pack.data_hdr.hdr_size = protocol::msg_hdr_size;
    pack.data_hdr.seq = 0;
    pack.data_hdr.type = msg_type::answer_msg;
    pack.data_hdr.state = msg_state::undef;
    pack.data_hdr.num_pck = (gen_numbers / number_val_in_pack)
                                   + ((gen_numbers % number_val_in_pack) ? 1 : 0);

    for(size_t i = 0; i < gen_numbers; ++i, ++count)
    {
        int64_t v = (int64_t)distribute( gen );
        v = v > pack.data_hdr.req_num ? ((v * unique_val.delimeter)/2) * -1 : v * unique_val.delimeter;

        pack.data_hdr.data[count] = v;

        if(number_val_in_pack == count)
        {
            update_hdr(count);
            pack.data_hdr.state = pack.data_hdr.state == msg_state::undef ? msg_state::beg : msg_state::in;
            count = 0;
            unique_val.generate(pack.data_hdr.data, number_val_in_pack);
            send_data();
            clean_unique_numbers();
        }
    }

    pack.data_hdr.state = msg_state::fin;
    if(count)
    {
        update_hdr(count);
        send_data();
    }
}

void generator::update_hdr(size_t count) noexcept
{
    ++pack.data_hdr.seq;
    --pack.data_hdr.num_pck;
    pack.data_hdr.size = protocol::msg_hdr_size + (count * sizeof(uint64_t));
}

void generator::operator()() noexcept
{
    while(running)
    {
        if(init())
            generate_unique_value();
    }
}

void generator::stop() noexcept
{
    receiver.notify_all();
    running = false;

    if(running.exchange(false))
    {
        if(job.joinable())
            job.join();
    }
}
    
}