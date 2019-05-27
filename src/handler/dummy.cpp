#include "dummy.h"
#include "../session.h"
#include "../server.h"

void handler_dummy::run(std::shared_ptr<redisReply> req
    , std::shared_ptr<session> res
    , coroutine_handler& ch) {

    res->reply_ok(ch);
}

// if(req->elements < 2) {
//     res->write("-ERR wrong number of arguments for 'auth' command\r\n", 51, ch);
//     return;
// }
// int index = std::strtol(req->element[1]->str, nullptr, 10);
// if(index < 0 || index > 256) {
//     res->write("-ERR DB index is out of range\r\n", 31, ch);
//     return;
// }
// res->db_ = server::get()->db(index);
