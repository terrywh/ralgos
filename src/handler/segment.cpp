#include "segment.h"
#include "../runner.h"
#include "../config.h"
#include "../session.h"
#include "../server.h"

handler_segment::handler_segment()
: jb_(G->cfg->jdict + "jieba.dict.utf8", G->cfg->jdict + "hmm_model.utf8",
    G->cfg->jdict + "user.dict.utf8", G->cfg->jdict + "idf.utf8",
    G->cfg->jdict + "stop_words.utf8") {

}

void handler_segment::run(std::shared_ptr<redisReply> req
    , std::shared_ptr<session> res
    , coroutine_handler& ch) {

    if(req->elements < 2) {
        res->write((boost::format("-ERR wrong number of arguments for command '%s'\r\n") % std::string_view(req->element[0]->str, req->element[0]->len)).str(), ch);
        return;
    }

    std::vector<std::string> rs;
    bool hmm = req->elements > 2 && req->element[2]->len == 3 && strncasecmp(req->element[2]->str, "HMM", 3) == 0;
    jb_.Cut(std::string(req->element[1]->str, req->element[1]->len), rs, hmm);
    std::stringstream ss;
    ss << '*' << rs.size() << '\r' << '\n';
    for(int i=0;i<rs.size();++i) {
        ss << '$' << rs[i].size() << '\r' << '\n' << rs[i] << '\r' << '\n';
    }
    std::string str = ss.str();
    res->write(ss.str(), ch);
}
