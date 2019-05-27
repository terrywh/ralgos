#include "config.h"

bool config::init(int argc, char* argv[]) {
	// 跟路径
	droot = std::filesystem::canonical(std::filesystem::absolute(argv[0])).parent_path().parent_path();
	// 选项定义
	boost::program_options::options_description desc("基于 Redis 协议的常用算法服务：");
	std::string
		http_addr = "127.0.0.1:8696",
		file_qqwry = droot.string() + "/var/qqwry.dat",
		dict_path = droot.string() + "/var";
	std::uint16_t workers = std::thread::hardware_concurrency()/2;

	desc.add_options()
			("help,h", "帮助信息")
			("c", boost::program_options::value<std::uint16_t>(&workers)->default_value(workers), "工作线程数量")
			("http",  boost::program_options::value<std::string>(&http_addr)->default_value(http_addr), "HTTP 服务监听端口")
            ("qqwry", boost::program_options::value<std::string>(&file_qqwry)->default_value(file_qqwry), "用于 IPv4 地址查询的纯真库")
			("jdict", boost::program_options::value<std::string>(&dict_path)->default_value(dict_path), "结巴分词词库目录");

	boost::program_options::variables_map vm;
	try{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	}catch(const std::exception& ex) {
		std::cerr << desc << std::endl;
		return false;
	}
	boost::program_options::notify(vm); // 外部变量更新通知

	if(vm.count("help") > 0) {
		std::cerr << desc << std::endl;
		return false;
	}
	// 工作线程
	wrkct = workers;
    // 地址处理
	auto x = http_addr.find_last_of(':');
	address.address(boost::asio::ip::make_address(http_addr.substr(0, x)));
	std::uint16_t port = std::atoi(http_addr.c_str() + x + 1);
	if(port == 0) {
		port = 8696;
	}
	address.port(port);
    // 路径处理
    std::error_code ec;
    std::filesystem::status(file_qqwry, ec);
    if(ec) {
        std::cerr << "[错误] 指定的 qqwry.dat 路径不存在或无法访问：" << ec << std::endl;
		return false;
    }
    qqwry = file_qqwry;
	// 目录处理
	if(*dict_path.rbegin() != '/') dict_path.push_back('/');
	
	std::filesystem::status(dict_path + "hmm_model.utf8", ec);
	if(ec) {
		std::cerr << "[错误] 指定的词库路径中不存在 hmm_model.utf8 文件或无法访问：" << ec << std::endl;
		return false;
	}
	std::filesystem::status(dict_path + "hmm_model.utf8", ec);
	if(ec) {
		std::cerr << "[错误] 指定的词库路径中不存在 hmm_model.utf8 文件或无法访问：" << ec << std::endl;
		return false;
	}
	std::filesystem::status(dict_path + "idf.utf8", ec);
	if(ec) {
		std::cerr << "[错误] 指定的词库路径中不存在 idf.utf8 文件或无法访问：" << ec << std::endl;
		return false;
	}
	std::filesystem::status(dict_path + "jieba.dict.utf8", ec);
	if(ec) {
		std::cerr << "[错误] 指定的词库路径中不存在 jieba.dict.utf8 文件或无法访问：" << ec << std::endl;
		return false;
	}
	std::filesystem::status(dict_path + "user.dict.utf8", ec);
	if(ec) {
		std::cerr << "[错误] 指定的词库路径中不存在 user.dict.utf8 文件或无法访问：" << ec << std::endl;
		return false;
	}
	jdict = dict_path;
	return true;
}
