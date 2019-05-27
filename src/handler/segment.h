#pragma once
#include "../vendor.h"
#include "../handler.h"

class handler_segment: public handler, public std::enable_shared_from_this<handler_segment> {
public:
    handler_segment();
    virtual void run(std::shared_ptr<redisReply> req, std::shared_ptr<session> res, coroutine_handler& ch);
private:
    cppjieba::Jieba jb_;
};
