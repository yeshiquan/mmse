#pragma once

#include "query.h"

namespace mmse {
 
class ReqOptSumScorer : public Scorer {
public:
    ReqOptSumScorer();
    ReqOptSumScorer(Scorer* req, Scorer* excl);
    ~ReqOptSumScorer();

    ReqOptSumScorer(ReqOptSumScorer const&) = delete;             // Copy construct
    ReqOptSumScorer(ReqOptSumScorer&&) = delete;                  // Move construct
    ReqOptSumScorer& operator=(ReqOptSumScorer const&) = delete;  // Copy assign
    ReqOptSumScorer& operator=(ReqOptSumScorer &&) = delete;      // Move assign    

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;    
    std::vector<std::string> explain() const override;
private:
    DocId _skip_to_nonexclude();
private:
    Scorer* _required_scorer{nullptr};
    Scorer* _optional_scorer{nullptr};
    DocId _current_doc = -1;
};

} // namespace