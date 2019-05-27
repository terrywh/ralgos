#pragma once
#include "vendor.h"

class config;
class server;
class runner;
extern runner* G;
class runner {
public:
    runner();
    ~runner();
    void init(int argc, char* argv[]);

    void run();
    boost::asio::io_context& ctx() {
        return *ctxs_[++ctxi_ % ctxs_.size()];
    }
    config* cfg;
    server* svr;
private:
    std::vector<std::thread>              pool_;
    std::vector<boost::asio::io_context*> ctxs_;
    std::atomic_int8_t                    ctxi_;
    std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
};
