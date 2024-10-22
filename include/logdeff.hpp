
#pragma once

#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <syncstream>
#include <time.h>

#define LOG_INFO( message... ) LogConfigSysLL::print(__FILE__, __FUNCTION__, "Info: ", message )
#define LOG_WARNING( message... ) LogConfigSysLL::print(__FILE__, __FUNCTION__, "Warning: ", message )
#define LOG_ERROR( message... ) LogConfigSysLL::print(__FILE__, __FUNCTION__, "Error: ", message )

constexpr static bool print_log{true};

class LogConfigSysLL{
public:
static inline void print(const char * i_file
                      , const char * i_method_name
                      , const char * i_message_head_name
                      , const char * i_message, ...)
{
    if constexpr (print_log)
    {
        const unsigned int sz = 4096;
        char buf[sz];
        buf[0] = 0;
        va_list ap;
        va_start ( ap, i_message );
        vsnprintf( buf, sz, i_message, ap );
        va_end ( ap );
        time_t t = time(0);
        struct tm* day = gmtime(&t);

        timespec ts = { 0 };
        clock_gettime(CLOCK_REALTIME, &ts);
        char buftm[128];
        snprintf(buftm, 128, "%04d-%02d-%02d %02d:%02d:%02d.%06lu",
                    day->tm_year + 1900,
                    day->tm_mon + 1,
                    day->tm_mday,
                    day->tm_hour,
                    day->tm_min,
                    day->tm_sec,
                    ts.tv_nsec/1000);


        std::string currtm(buftm);
        std::osyncstream(std::cout) << currtm << ": " << std::string(i_file) << "->" << std::string(i_method_name) << "(): " << i_message_head_name << buf << std::endl;
    }
}

};
