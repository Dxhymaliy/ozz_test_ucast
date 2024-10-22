#pragma once

// #include <cerrno>
// #include <sys/ioctl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/time.h>
// #include <sys/wait.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <net/if.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <unistd.h>
// #include <time.h>
// #include <glob.h>
// #include <pthread.h>
// #include <sched.h>
// #include <semaphore.h>
// #include <signal.h>
// #include <dlfcn.h>
// #include <pwd.h>



// #include <limits.h>



#include <string>
// #include <vector>
// #include <array>
// #include <deque>
// #include <list>
// #include <map>
// #include <set>
// #include <bitset>
// #include <algorithm>
// #include <iterator>
// #include <limits>
// #include <locale>
// #include <istream>
// #include <ostream>
// #include <fstream>
// #include <sstream>
// #include <iostream>
// #include <exception>
// #include <typeinfo>

// #include <cassert>
// #include <cstdlib>
// #include <cstdarg>
// #include <cstdio>
// #include <cstdint>
#include <cstring>
#include <cctype>

#if(defined(_WIN32) || defined(_WIN64))
#include <winsock2.h>
#include <ws2tcpip.h>
typedef ::SOCKET SOCKET_T;
#else
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
typedef int SOCKET_T;
#endif

#define OZZ_CORE ozz
#define OZZ_NET OZZ_CORE::net

namespace OZZ_CORE{

using milli_sec = uint16_t;
constexpr milli_sec milli_sec_per_sec{1000};
constexpr std::size_t max_mtu{1200};

inline static long long get_number_from_json_str(const std::string& str, std::string key)
{
    const std::string beg{'\"'};
    const std::string end{"\":"};
    key = beg + key + end;
    auto pos = str.find(key);
    if(pos != std::string::npos)
        return std::stoll(str.substr(pos + key.size()));

    return 0;
}

inline static ushort get_port_from_json_str(const std::string& str)
{ return get_number_from_json_str(str, "port"); }

inline static std::string get_ip_from_json_str(const std::string& str)
{
    std::string key{R"("ip":")"};
    char end{'\"'};
    auto pos = str.find(key);
    if(pos != std::string::npos)
    {
        const char* buf = str.data() + pos + key.size();
        size_t len = 0;
        while (*(buf + len) != end)
            ++len;
        
        return str.substr(pos + key.size(), len);
    }

    return {};
}

inline static  void init_delimeter(uint64_t num, uint& res)
{
    if(num >= 1000'000'000)
        res = 100;
    if(num >= 100'000'000)
        res = 1'000;
    if(num >= 10'000'000)
        res = 10'000;
    else 
        res = 100'000;
}

}

