#include "../runner.h"
#include "address.h"
#include "../session.h"
#include "../server.h"
#include "../config.h"

handler_address::handler_address()
: qq_(G->cfg->qqwry) {}

void handler_address::run(std::shared_ptr<redisReply> req
    , std::shared_ptr<session> res
    , coroutine_handler& ch) {

    if(req->elements < 2) {
        res->write(( boost::format("-ERR wrong number of arguments for command '%s'\r\n") % std::string_view(req->element[0]->str, req->element[0]->len) ).str(), ch);
        return;
    }
    std::string_view sv {req->element[1]->str, req->element[1]->len};
    auto rs = qq_.search(sv);
    res->write((boost::format("*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n")
        % rs.first.size() % rs.first % rs.second.size() % rs.second).str(), ch);
}
