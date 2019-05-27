#pragma once
#include "vendor.h"


class coroutine;
class coroutine_handler {
public:
    coroutine_handler();
    coroutine_handler(std::shared_ptr<coroutine> co);
    coroutine_handler(const coroutine_handler& ch) = default;
    coroutine_handler& operator[](boost::system::error_code& error);
    inline void operator()(const boost::system::error_code& error, std::size_t size = 0) {
        if(error_) *error_ = error;
        if(size_) *size_ = size;
        resume();
    }
    void reset();
    void reset(std::shared_ptr<coroutine> co);

    void suspend();
    void resume();
public:
    std::size_t* size_;
    std::shared_ptr<boost::system::error_code> error_;
    std::shared_ptr<coroutine> co_;

private:

};

class coroutine {
public:
    template <class Executor, class Handler>
    coroutine(Executor&& ex, Handler&& fn)
    : ex_(ex)
    , fn_(fn) {
        
    }

    // static std::shared_ptr<coroutine> current;
    template <class Executor, class Handler>
    static void start(Executor&& et, Handler&& fn) {
        auto co = std::make_shared<coroutine>(et, fn);
        
        boost::asio::post(co->ex_, [co] () mutable {
            co->id_ = std::this_thread::get_id();
            co->c1_ = boost::context::fiber([co, gd = boost::asio::make_work_guard(co->ex_)] (boost::context::fiber&& c2) mutable {
                coroutine_handler ch(co);
                co->c2_ = std::move(c2);

                co->fn_(ch);
                co->fn_ = nullptr;
                return std::move(co->c2_);
            });
            // coroutine::current = co;
            co->c1_ = std::move(co->c1_).resume();
        });
    }
private:
    boost::context::fiber c1_;
    boost::context::fiber c2_;
    boost::asio::executor ex_;
    std::function<void (coroutine_handler ch)> fn_;
    std::thread::id id_;
    friend class coroutine_handler;
};

namespace boost::asio {
    template <>
    class async_result<::coroutine_handler, void (boost::system::error_code error, std::size_t size)> {
    public:
        explicit async_result(::coroutine_handler& ch) : ch_(ch), size_(0) {
            ch_.size_ = &size_;
        }
        using completion_handler_type = ::coroutine_handler;
        using return_type = std::size_t;
        return_type get() {
            ch_.suspend();
            return size_;
        }
    private:
        ::coroutine_handler &ch_;
        std::size_t size_;
    };

    template <>
    class async_result<::coroutine_handler, void (boost::system::error_code error)> {
    public:
        explicit async_result(::coroutine_handler& ch) : ch_(ch) {
        }
        using completion_handler_type = ::coroutine_handler;
        using return_type = void;
        void get() {
            ch_.suspend();
        }
    private:
        ::coroutine_handler &ch_;
    };
} // namespace boost::asio
