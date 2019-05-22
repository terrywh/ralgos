#include "coroutine.h"

coroutine_handler::coroutine_handler()
: size_(nullptr) {}

coroutine_handler::coroutine_handler(std::shared_ptr<coroutine> co)
: co_(co)
, size_(nullptr) {

}

coroutine_handler& coroutine_handler::operator[](boost::system::error_code& error) {
    error_.reset(&error, boost::null_deleter());
    return *this;
}

void coroutine_handler::resume() {
    // 恢复实际执行的上下文，可能需要对应执行器（线程）
    std::shared_ptr<coroutine> co = co_;
    boost::asio::post(co_->ex_, [co] () {
        coroutine::current = co;
        co->c1_ = std::move(co->c1_).resume();
    });
}

void coroutine_handler::reset(std::shared_ptr<coroutine> co) {
    co_ = co;
    error_.reset();
    size_ = nullptr;
}

void coroutine_handler::reset() {
    co_.reset();
    error_.reset();
    size_ = nullptr;
}

void coroutine_handler::suspend() {
    // 暂定本身就位于当前执行器（线程）
    coroutine::current.reset();
    co_->c2_ = std::move(co_->c2_).resume();
}

std::shared_ptr<coroutine> coroutine::current;
