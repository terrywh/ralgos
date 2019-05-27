#include "session.h"
#include "runner.h"
#include "server.h"
#include "handler.h"

session::session(boost::asio::io_context& io)
: socket_(io)
, reader_(redisReaderCreate())
, writer_(io) {

}

session::~session() {
    redisReaderFree(reader_);
}

void session::run(coroutine_handler ch) {
    boost::system::error_code error;

    while(!error) {
        char data[2048];
        redisReply* reply = nullptr;
        std::size_t size = 0;
        size = socket_.async_read_some(boost::asio::buffer(data), ch[error]);
        if(error == boost::asio::error::eof || error == boost::asio::error::connection_reset) return;
        redisReaderFeed(reader_, data, size); // will copy data/size
        while(REDIS_OK == redisReaderGetReply(reader_, (void**)&reply)) {
            if(reply == nullptr) break;
            if(!handle_request(reply)) goto REQUEST_TYPE_ERROR;
        }
    }
    if(error) {
        // std::cerr << "failed to read from socket: " << error.message() << "\n";
    }
    else if(reader_->err) {
        // std::cerr << "failed to parse redis proto: " << reader_->errstr << "\n";
    }
REQUEST_TYPE_ERROR:
    write({"-WRONGTYPE Array of at least 1 Bulk String is required\r\n", 56}, ch);
}

static std::string upper(char* cmd, std::size_t len) {
    std::string u(cmd, len);
    std::transform(u.begin(), u.end(), u.begin(), [](unsigned char c) { return std::toupper(c); });
    return std::move(u);
}

bool session::handle_request(redisReply* reply) {
    static std::string UNKONWN_COMMAND = "UNKNOWN";

    std::shared_ptr<redisReply> req {reply, freeReplyObject}; // 自动释放
    boost::system::error_code error;
    // https://redis.io/topics/protocol
    // RESP Array consisting of just Bulk Strings
    switch(req->type) {
    case REDIS_REPLY_ARRAY:
        if(req->elements < 1) return false;
    break;
    default:
        return false;
    }
    for(int i=0;i<req->elements;++i) {
        if(req->element[i]->type != REDIS_REPLY_STRING) return false;
    }
    std::string cmd = upper(req->element[0]->str, req->element[0]->len);
    // std::shared_ptr<handler> h = G->svr->get(cmd);
    // h->handle(req, shared_from_this(), ch);
    coroutine::start(G->ctx().get_executor(), [h = G->svr->get(cmd), req, self = shared_from_this()] (coroutine_handler ch) {
        h->run(req, self, ch);
    });
    return true;
}

void session::write(std::string&& cbs, coroutine_handler& ch) {
    // 若 使用 socket_.get_excutor() 代替 writer 时, 可能产生如下时序:
    // (1) [size == 1] -> (2) -> (3) |->| (1) size == 1 -> (3)
    //                            |
    //                           (2) -> (3)
    // 导致 (3) 被执行的次数超过预期(同时发送的数据可能重复)
    boost::asio::post(writer_, [this, self = shared_from_this(), &ch, cbs = std::move(cbs)] () {
        // (1)
        wqueue_.emplace_back(std::move(cbs));
        if(wqueue_.size() == 1) write_next();
        ch.resume();
    });
    ch.suspend();
}

void session::write_next() { // (2)
    if(wqueue_.empty()) return;
    std::string& cbs = wqueue_.front();
    boost::asio::async_write(socket_, boost::asio::buffer(cbs), [this, self = shared_from_this()] (const boost::system::error_code& error, std::size_t nsent) {
        // if(error && error != boost::asio::error::eof && error != boost::asio::error::connection_reset)
        //     std::cerr << "failed to write reply: (" << error.value() << ") " << error.message() << "\n";
        wqueue_.pop_front(); // (3)
        boost::asio::post(writer_, std::bind(&session::write_next, shared_from_this()));
    });
}

void session::reply(const char* val, std::size_t len, coroutine_handler& ch) {
    write((boost::format("+%s\r\n") % std::string_view(val, len)).str(), ch);
}

void session::reply(std::string_view val, coroutine_handler& ch) {
    write((boost::format("$%ld\r\n%s\r\n") % val.size() % val).str(), ch);
}

void session::reply(int64_t val, coroutine_handler& ch) {
    write((boost::format(":%ld\r\n") % val).str(), ch);
}

void session::reply_error(std::string_view val, coroutine_handler& ch) {
    write((boost::format("-%s\r\n") % val).str(), ch);
}

void session::reply_ok(coroutine_handler& ch) {
    write({"+OK\r\n", 5}, ch);
}
