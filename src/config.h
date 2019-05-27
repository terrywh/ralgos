#pragma once
#include "vendor.h"

class config {
public:
	std::filesystem::path            droot;
	std::uint16_t                    wrkct;
	boost::asio::ip::tcp::endpoint address;
	std::filesystem::path            qqwry;
	std::string                      jdict;
	bool init(int argc, char* argv[]);
	bool                           closing = false;
};
