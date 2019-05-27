#pragma once
#include "vendor.h"
#include "coroutine.h"

class session;
class handler;

class server: public std::enable_shared_from_this<server> {
public:
    server();
    void run(coroutine_handler ch);
    std::shared_ptr<handler> get(const std::string& cmd);
private:
    boost::asio::ip::tcp::acceptor  acc_;
    std::map<std::string, std::shared_ptr<handler>> hdl_;
    std::shared_ptr<handler> hdl_unknown_;

    friend class database;
};
