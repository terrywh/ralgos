#include "qqwry.h"
#include <thread>
#include <iostream>

qqwry::qqwry(std::filesystem::path filepath) {
    mmfd_ = ::open(filepath.c_str(), O_RDONLY);
    ::fstat(mmfd_, &mmst_);
    mmap_ = (struct mmap_t*)::mmap(nullptr, mmst_.st_size, PROT_READ, MAP_PRIVATE, mmfd_, 0);
    micv_ = iconv_open("UTF-8//TRANSLIT", "GB18030");
}

qqwry::~qqwry() {
    ::munmap(mmap_, mmst_.st_size);
    ::close(mmfd_);
    iconv_close(micv_);
}

std::pair<std::string, std::string> qqwry::search(std::uint32_t ipv4) {
    std::string country, address;
    mmap_one_t* one = index(ipv4), *two;
    if(!one) return {"", ""};
    switch(one->mode) {
    case 0x02:
        std::tie(country, two) = fetch(reinterpret_cast<mmap_one_t*>((char*)mmap_ + one->offset));
        std::tie(address, one) = fetch(reinterpret_cast<mmap_one_t*>(one->data + 4));
        break;
    case 0x01:
        std::tie(country, one) = fetch(reinterpret_cast<mmap_one_t*>((char*)mmap_ + one->offset));
        std::tie(address, one) = fetch(one);
        break;
    default:
        std::tie(country, one) = fetch(one);
        std::tie(address, one) = fetch(one);
    }
    return {country, address};
}

std::string qqwry::version() {
    auto v = search(convert("255.255.255.255"));
    return v.first + "/" + v.second;
}

std::uint32_t qqwry::convert(const char* addr) {
    std::uint32_t ipv4;
    inet_pton(AF_INET, addr, &ipv4);
    return htonl(ipv4);
}

std::uint32_t qqwry::convert(std::string_view addr) {
    return convert(addr.data());
}

std::string qqwry::convert(std::uint32_t addr) {
    std::stringstream ss;
    ss << ((addr & 0xff000000) >> 24);
    ss << ".";
    ss << ((addr & 0x00ff0000) >> 16);
    ss << ".";
    ss << ((addr & 0x0000ff00) >> 8);
    ss << ".";
    ss <<  (addr & 0x000000ff);
    return ss.str();
}

qqwry::mmap_one_t* qqwry::index(std::uint32_t ipv4) {
    mmap_idx_t* begin = reinterpret_cast<mmap_idx_t*>((char*)mmap_ + mmap_->begin),
        * end = reinterpret_cast<mmap_idx_t*>((char*)mmap_ + mmap_->end);

    while(end > begin + 1) { // 二分查找
        // std::cout << convert(begin->address) << " ~ " << convert(end->address) << "\n";
        mmap_idx_t* x = begin + (end - begin) / 2;
        if(x->address <= ipv4) begin = x;
        else end = x;
    }
    if(ipv4 >= begin->address && ipv4 <= end->address)
        return reinterpret_cast<mmap_one_t*>((char*)mmap_ + begin->offset + 4);
    else if(ipv4 >= end->address)
        return reinterpret_cast<mmap_one_t*>((char*)mmap_ + end->offset + 4);
    return nullptr;
}

std::pair<std::string, qqwry::mmap_one_t*> qqwry::fetch(qqwry::mmap_one_t* one) {
    switch(one->mode) {
    case 0x02:
    case 0x01:
        return {
            fetch( reinterpret_cast<mmap_one_t*>((char*)mmap_ + one->offset) ).first,
            reinterpret_cast<mmap_one_t*>((char*)one + 4),
        };
    break;
    default: {
        std::string_view src {one->data};
        std::string dst;
        if(src.compare(" CZ88.NET") != 0) {
            dst.resize(src.size() * 1.5 + 1, '\0');
            char* csrc = one->data, *cdst = dst.data();
            std::size_t ssrc = src.size(), sdst = dst.capacity();
            iconv_t ccvt = iconv_open("UTF-8", "GB18030");
            iconv(ccvt, &csrc, &ssrc, &cdst, &sdst);
            // iconv(ccvt, nullptr, nullptr, &cdst, &sdst);
            iconv_close(ccvt);
            dst.resize(dst.capacity() - sdst);
            // dst.shrink_to_fit();
        }
        return {
            // c1.to_bytes(c, c + str.size() / 2),
            std::move(dst),
            reinterpret_cast<mmap_one_t*>(one->data + src.size() + 1),
        };
    }
    }
}
