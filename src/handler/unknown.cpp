#include "unknown.h"
#include "../session.h"

void handler_unknown::run(std::shared_ptr<redisReply> req
    , std::shared_ptr<session> res
    , coroutine_handler& ch) {

    res->write((boost::format("-ERR unknown command '%s'\r\n") % std::string_view(req->element[0]->str, req->element[0]->len)).str(), ch);
}
