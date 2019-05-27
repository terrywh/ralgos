#pragma once
#include "vendor.h"
#include "coroutine.h"
#include "session.h"

class session;
class handler {
public:
    virtual ~handler() = default;
    virtual void run(std::shared_ptr<redisReply> req, std::shared_ptr<session> res, coroutine_handler& ch) = 0;

};
