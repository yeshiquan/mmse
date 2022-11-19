#include "req_opt_sum_scorer.h"

namespace unise {

ReqOptSumScorer::ReqOptSumScorer() {
}

ReqOptSumScorer::ReqOptSumScorer(Scorer* req, Scorer* opt)
        : _required_scorer(req)
        , _optional_scorer(opt) {
}

ReqOptSumScorer::~ReqOptSumScorer() {
}

DocId ReqOptSumScorer::next_doc() {
    return _required_scorer->next_doc();
}

DocId ReqOptSumScorer::skip_to(DocId target) {
    return _required_scorer->skip_to(target);
}

DocId ReqOptSumScorer::doc() const {
    return _required_scorer->doc();
}

std::vector<std::string> ReqOptSumScorer::explain() const {
    std::vector<std::string> outputs;
    outputs.emplace_back("ReqOptSumScorer ->");
    for (auto& line : _required_scorer->explain()) {
        outputs.emplace_back("  " + line);
    }
    for (auto& line : _optional_scorer->explain()) {
        outputs.emplace_back("  " + line);
    }    
    return outputs;
}

} // namespace