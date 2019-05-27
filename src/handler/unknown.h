#pragma once
#include "../vendor.h"
#include "../handler.h"

class handler_unknown: public handler, public std::enable_shared_from_this<handler_unknown> {
    virtual void run(std::shared_ptr<redisReply> req, std::shared_ptr<session> res, coroutine_handler& ch);
};
