#include "ozz/net/server.hpp"
#include "ozz/utils/mcs.hpp"


int main()
{
    std::string conf{"conf.json"};
    OZZ_CORE::mcs<OZZ_NET::unicast::server> srv;

    if(!srv.start(conf))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
