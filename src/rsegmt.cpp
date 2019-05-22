#include "vendor.h"

int main(int argc, char* argv[]) {
    std::string sentence = "基于 cppjieba 提供分词服务";
    std::string dictpath = "../var";
    boost::program_options::options_description desc(sentence);
    desc.add_options()
			("help,h", "帮助信息")
            ("dict,d", boost::program_options::value<std::string>(&dictpath)->default_value(dictpath), "词库路径")
            ("sentence", boost::program_options::value<std::string>(&sentence)->default_value(sentence), "目标语句");
    boost::program_options::variables_map vm;
    boost::program_options::positional_options_description pn;
    pn.add("sentence", -1);
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
            .options(desc).positional(pn).run(), vm);
    boost::program_options::notify(vm);

    if(vm.count("help") > 0 || argc < 2) {
        std::cout << "用法: rsegmt {目标语句}\n";
        return 1;
    }
    
    if(*dictpath.rbegin() != '/') dictpath.push_back('/');
    std::cout << "加载词库: " << dictpath << "\n";
    cppjieba::Jieba jb(
        dictpath + "jieba.dict.utf8",
        dictpath + "hmm_model.utf8",
        dictpath + "user.dict.utf8",
        dictpath + "idf.utf8",
        dictpath + "stop_words.utf8");
    std::cout << "分词：\n";
    std::vector<std::string> rs;
    jb.Cut(sentence, rs, true);
    int x = 0;
    for(auto i=rs.begin();i!=rs.end();++i) {
        if((++x) > 1) std::cout << "/";
        std::cout << *i;
    }
    std::cout << "\n";
    return 0;
}
