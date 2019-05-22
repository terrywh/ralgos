#include "vendor.h"
#include "qqwry.h"

int main(int argc, char* argv[]) {
    qqwry x("/home/wuhao/Downloads/qqwry_lastest.dat");

    std::string address;
    boost::program_options::options_description desc("纯真 IPv4 地址库查询：");
    desc.add_options()
			("help,h", "帮助信息")
            ("v", "版本信息")
            ("addr", boost::program_options::value<std::string>(&address)->default_value("127.0.0.1"), "目标地址");

	boost::program_options::variables_map vm;
    boost::program_options::positional_options_description pn;
    pn.add("addr", -1);
	boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
            .options(desc).positional(pn).run(), vm);
    if(vm.count("help") > 0 || argc < 2) {
        std::cout << "用法: raddrs {目标地址}\n";
        return 1;
    }

    if(std::string_view(argv[1]).compare("-v") == 0) {
        std::cout << x.version() << "\n";
        return 0;
    }

    std::uint32_t src = x.convert(argv[1]);
    std::cout << x.convert(src) << " => " << src << "\n";
    auto rst = x.search(src);
    std::cout << "(" << rst.first.length() << ") " << rst.first << " : (" << rst.second.length() << ") " << rst.second << "\n";
    return 0;
}
