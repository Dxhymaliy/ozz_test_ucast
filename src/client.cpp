#include "ozz/net/client.hpp"
#include "ozz/utils/mcs.hpp"


int main()
{
    std::string conf{"conf.json"};
    OZZ_CORE::mcs<OZZ_NET::unicast::client> cl;

    if(!cl.start(conf))
        return EXIT_FAILURE;

    // cl.run();

    return EXIT_SUCCESS;
}