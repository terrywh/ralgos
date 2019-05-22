#pragma once
#include "vendor.h"
#include "coroutine.h"

class session: public std::enable_shared_from_this<session> {
public:
    session(boost::asio::io_context& io);
    ~session();

    void run(coroutine_handler ch);
    template <class ConstBufferSequence>
    void write(ConstBufferSequence && cbs, coroutine_handler& ch) {
        boost::asio::async_write(socket_, cbs, [this, &ch] (const boost::system::error_code& error, std::size_t nsent) {
            if(error) std::cout << "failed to write reply: (" << error.value() << ") " << error.message() << "\n";
            ch.resume();
        });
        ch.suspend();
    }
    void reply(const char* val, std::size_t len, coroutine_handler& ch);
    void reply(std::string_view val, coroutine_handler& ch);
    void reply(int64_t val, coroutine_handler& ch);
    void reply_ok(coroutine_handler& ch);
    void reply_error(std::string_view val, coroutine_handler& ch);

protected:
    bool handle_request(redisReply* req, coroutine_handler& ch);
private:
    boost::asio::executor*      context_;
    boost::asio::ip::tcp::socket socket_;
    redisReader*                 reader_;
    char                         buffer_[256];
    // 单个 session 不会出现并行写入的场景
    // boost::asio::io_context::strand writer_; // 对 socket_ 写入的保护

    friend class server;
    friend class handler;
    friend class handler_funnel;
    friend class handler_select;
};
