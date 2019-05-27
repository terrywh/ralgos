#pragma once
#include "vendor.h"
#include "coroutine.h"

class session: public std::enable_shared_from_this<session> {
public:
    session(boost::asio::io_context& io);
    ~session();

    void run(coroutine_handler ch);
    void write(std::string&& cbs, coroutine_handler& ch);
    void reply(const char* val, std::size_t len, coroutine_handler& ch);
    void reply(std::string_view val, coroutine_handler& ch);
    void reply(int64_t val, coroutine_handler& ch);
    void reply_ok(coroutine_handler& ch);
    void reply_error(std::string_view val, coroutine_handler& ch);

protected:
    bool handle_request(redisReply* req);
private:
    boost::asio::ip::tcp::socket socket_;
    redisReader*                 reader_;
    boost::asio::io_context::strand writer_; // 对 socket_ 写入的保护
    std::list<std::string>          wqueue_;

    void write_next();
    friend class server;
    friend class handler;
    friend class handler_funnel;
    friend class handler_select;
};
