#include "runner.h"
#include "config.h"
#include "server.h"

runner* G = nullptr;

runner::runner() {
    G = this;
}

runner::~runner() {
    for(std::size_t i=0;i<pool_.size();++i) {
        delete ctxs_[i];
    }
}

void runner::init(int argc, char* argv[]) {
    cfg = new config();
    if(!cfg->init(argc, argv)) exit(-1);
    pool_.resize(cfg->wrkct);
    for(std::size_t i=0;i<pool_.size();++i) {
        ctxs_.push_back( new boost::asio::io_context() );
        work_.push_back( boost::asio::make_work_guard(*ctxs_[i]) );
    }
    svr = new server();
    coroutine::start(ctx().get_executor(), std::bind(&server::run, svr, std::placeholders::_1));
}

void runner::run() {
    for(std::size_t i=0;i<pool_.size();++i) {
        pool_[i] = std::thread([i] (runner* self) {
            self->ctxs_[i]->run();
        }, this);
    }
    for(std::size_t i=0;i<pool_.size();++i) {
        pool_[i].join();
    }
}
