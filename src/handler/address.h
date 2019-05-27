#pragma once
#include "../vendor.h"
#include "../handler.h"
#include "../qqwry.h"

class handler_address: public handler, public std::enable_shared_from_this<handler_address> {
public:
    handler_address();
    virtual void run(std::shared_ptr<redisReply> req, std::shared_ptr<session> res, coroutine_handler& ch);
private:
    qqwry qq_;
};
