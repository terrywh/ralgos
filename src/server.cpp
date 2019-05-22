#include "server.h"
#include "runner.h"
#include "config.h"
#include "session.h"
#include "handler.h"
#include "handler/unknown.h"
#include "handler/dummy.h"
#include "handler/address.h"
#include "handler/segment.h"

server::server()
: acc_(G->ctx(), G->cfg->address) {
    // 初始化各命令处理器
    hdl_["UNKNOWN"] = std::make_shared<handler_unknown>();
    std::shared_ptr<handler> dummy = std::make_shared<handler_dummy>();
    hdl_["AUTH"]    = dummy;
    hdl_["SELECT"]  = dummy;
    hdl_["ADDRESS"] = std::make_shared<handler_address>();
    hdl_["SEGMENT"] = std::make_shared<handler_segment>();
}

void server::run(coroutine_handler ch) {
    acc_.listen();
    std::shared_ptr<session> ss;
    boost::system::error_code error;
    while(!error) {
        boost::asio::io_context& ctx = G->ctx();
        ss  = std::make_shared<session>(ctx);
        acc_.async_accept(ss->socket_, ch[error]);
        if(!error) coroutine::start(ctx.get_executor(),
            std::bind(&session::run, ss, std::placeholders::_1));
    }
}

std::shared_ptr<handler> server::get(const std::string& cmd) {
    static std::string UNKONWN_COMMAND = "UNKNOWN";
    auto hfind = hdl_.find(cmd);
    return hfind == hdl_.end() ? hdl_[UNKONWN_COMMAND] : hfind->second;
}
