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
        if(++ctxi_ >= ctxs_.size()) ctxi_ = 0;
        return *ctxs_[ctxi_];
    }
    config* cfg;
    server* svr;
private:
    std::vector<std::thread>             pool_;
    std::vector<boost::asio::io_context*> ctxs_;
    std::size_t                          ctxi_;
    std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
};
