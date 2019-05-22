#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include <string>
#include <cstring>
#include <iconv.h>

class qqwry {
public:
    struct mmap_t {
        std::uint32_t begin;
        std::uint32_t end;
    };
    struct __attribute__((packed)) mmap_idx_t {
        std::uint32_t address;
        std::uint32_t offset : 24;
    };
    union mmap_one_t {
        struct {
            char mode;
            std::uint32_t offset : 24;
        };
        char data[0];
    };

    qqwry(std::filesystem::path filepath);
    ~qqwry();
    std::pair<std::string, std::string> search(std::string_view addr) {
        return search(convert(addr));
    }
    std::pair<std::string, std::string> search(std::uint32_t ipv4);
    static std::uint32_t convert(const char* addr);
    static std::uint32_t convert(std::string_view addr);
    static std::string convert(std::uint32_t addr);
    std::string version();

private:
    template <typename T>
    inline T* offset(std::uint32_t offset) {
        return reinterpret_cast<T*>((char*) mmap_ + offset);
    }
    qqwry::mmap_one_t* index(std::uint32_t ipv4);
    std::pair<std::string, mmap_one_t*> fetch(mmap_one_t* rec);

    // std::ifstream ifs_;
    // std::uint32_t begin_;
    // std::uint32_t end_;
    int            mmfd_;
    struct stat    mmst_;
    struct mmap_t* mmap_;
    iconv_t        micv_;
};
