//microservice general implementation 
#pragma once

#include<signal.h>
#include <memory>
#include <fstream> 
#include <functional>

#include "ozz/types.hpp"


namespace OZZ_CORE{

std::function<void()> sig_handler_clb;
void signal_handler(int signum)
{
    LOG_INFO("Interrupt signal %d received", signum);
    if (sig_handler_clb) {
        sig_handler_clb();
    }
}

template<typename T>
class mcs
{
public:
    bool start(const std::string& path)
    {
        std::string conf;
        std::ifstream ifs(path.c_str());

        if(!ifs)
        {
            LOG_ERROR("Cannot read config file");
            return false;
        }
        //"{"ip":"127.0.0.1","port":50100,"request_number":100}"
        ifs >> conf;
        LOG_INFO("Loaded AF config '%s' ", conf.c_str());

        ifs.close();

        worker = std::make_unique<T>(conf);

        if(!worker->init())
        {
            LOG_ERROR("Cannot initialize worker");
            return false;
        }

        sig_handler_clb = [this]() { stop(); };

        if(SIG_ERR == signal(SIGINT, signal_handler))
            LOG_WARNING("setting signal handler (SIGINT) failed");

        if(SIG_ERR == signal(SIGTERM, signal_handler)) 
            LOG_WARNING("setting signal handler (SIGTERM) failed");
        worker->run(); 
        return true;
    }

    void stop()
    { worker->stop(); }

private:
        
    std::unique_ptr<T> worker;
};

}