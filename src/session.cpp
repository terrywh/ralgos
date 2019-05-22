#include "session.h"
#include "runner.h"
#include "server.h"
#include "handler.h"

session::session(boost::asio::io_context& io)
: socket_(io)
, reader_(redisReaderCreate())
/*, writer_(io) */{

}

session::~session() {
    redisReaderFree(reader_);
}

void session::run(coroutine_handler ch) {
    char data[2048];
    redisReply* reply = nullptr;
    std::size_t size = 0;
    boost::system::error_code error;

    while(!error) {
        size = socket_.async_read_some(boost::asio::buffer(data), ch[error]);
        if(error == boost::asio::error::eof || error == boost::asio::error::connection_reset) return;
        redisReaderFeed(reader_, data, size);
        if(REDIS_OK != redisReaderGetReply(reader_, (void**)&reply)) break;
        if(reply != nullptr && !handle_request(reply, ch)) goto REQUEST_TYPE_ERROR;
    }

    if(error) {
        std::cerr << "failed to read from socket: " << error.message() << "\n";
    }
    else if(reader_->err) {
        std::cerr << "failed to parse redis proto: " << reader_->errstr << "\n";
    }
REQUEST_TYPE_ERROR:
    write(boost::asio::buffer("-WRONGTYPE Array of at least 2 Bulk String is required\r\n", 56), ch);
}

static std::string upper(char* cmd, std::size_t len) {
    std::string u(cmd, len);
    std::transform(u.begin(), u.end(), u.begin(), [](unsigned char c) { return std::toupper(c); });
    return std::move(u);
}

bool session::handle_request(redisReply* reply, coroutine_handler& ch) {
    static std::string UNKONWN_COMMAND = "UNKNOWN";

    std::shared_ptr<redisReply> req {reply, freeReplyObject}; // 自动释放
    boost::system::error_code error;
    // https://redis.io/topics/protocol
    // RESP Array consisting of just Bulk Strings
    switch(req->type) {
    case REDIS_REPLY_ARRAY:
        if(req->elements < 2) return false;
    break;
    default:
        return false;
    }
    for(int i=0;i<req->elements;++i) {
        if(req->element[i]->type != REDIS_REPLY_STRING) return false;
    }

    std::string cmd = upper(req->element[0]->str, req->element[0]->len);
    auto h = G->svr->get(cmd);
    coroutine::start(G->ctx().get_executor(), [h, req, this] (coroutine_handler ch) {
        h->handle(req, shared_from_this(), ch);
    });
    return true;
}

void session::reply(const char* val, std::size_t len, coroutine_handler& ch) {
    std::vector<boost::asio::const_buffer> data = {{"+", 1}, {val, len}, {"\r\n", 2}};
    write(data, ch);
}

void session::reply(std::string_view val, coroutine_handler& ch) {
    size_t length = sprintf(buffer_, "$%ld\r\n", val.length());
    std::vector<boost::asio::const_buffer> data = {{buffer_, length}, {val.data(), val.size()}, {"\r\n", 2}};
    write(data, ch);
}

void session::reply(int64_t val, coroutine_handler& ch) {
    size_t length = std::sprintf(buffer_, ":%ld\r\n", val);
    write(boost::asio::buffer(buffer_, length), ch);
}

void session::reply_error(std::string_view val, coroutine_handler& ch) {
    std::vector<boost::asio::const_buffer> data = {{"-", 1}, {val.data(), val.size()}, {"\r\n", 2}};
    write(data, ch);
}

void session::reply_ok(coroutine_handler& ch) {
    write(boost::asio::buffer("+OK\r\n", 5), ch);
}
