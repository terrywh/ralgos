#pragma once
#include "../vendor.h"
#include "../handler.h"

class handler_ping: public handler, public std::enable_shared_from_this<handler_ping> {
    virtual void run(std::shared_ptr<redisReply> req, std::shared_ptr<session> res, coroutine_handler& ch);
};
