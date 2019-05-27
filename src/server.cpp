#include "server.h"
#include "runner.h"
#include "config.h"
#include "session.h"
#include "handler.h"
#include "handler/unknown.h"
#include "handler/dummy.h"
#include "handler/ping.h"
#include "handler/address.h"
#include "handler/segment.h"

server::server()
: acc_(G->ctx(), G->cfg->address) {
    // 初始化各命令处理器
    hdl_unknown_ = std::make_shared<handler_unknown>();
    std::shared_ptr<handler> dummy = std::make_shared<handler_dummy>();
    hdl_["AUTH"]    = dummy;
    hdl_["SELECT"]  = dummy;
    hdl_["PING"]    = std::make_shared<handler_ping>();
    hdl_["ADDRESS"] = std::make_shared<handler_address>();
    hdl_["SEGMENT"] = std::make_shared<handler_segment>();
    // hdl_["ADDRESS"] = dummy;
    // hdl_["SEGMENT"] = dummy;
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
    auto hfind = hdl_.find(cmd);
    return hfind == hdl_.end() ? hdl_unknown_ : hfind->second;
}
