#include "unknown.h"
#include "../session.h"

void handler_unknown::handle(std::shared_ptr<redisReply> req
    , std::shared_ptr<session> res
    , coroutine_handler& ch) {

    std::string err = (boost::format("-ERR unknown command '%s'\r\n") % req->element[0]->str).str();
    res->write(boost::asio::buffer(err.data(), err.size()), ch);
}
