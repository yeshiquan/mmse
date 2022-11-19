#pragma once

#include "query.h"

namespace mmse {
 
// 实现差集, RequiredScorer - ExcludeScorer

class ReqExclScorer : public Scorer {
public:
    ReqExclScorer();
    ReqExclScorer(Scorer* req, Scorer* excl);
    ~ReqExclScorer();

    ReqExclScorer(ReqExclScorer const&) = delete;             // Copy construct
    ReqExclScorer(ReqExclScorer&&) = delete;                  // Move construct
    ReqExclScorer& operator=(ReqExclScorer const&) = delete;  // Copy assign
    ReqExclScorer& operator=(ReqExclScorer &&) = delete;      // Move assign    

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;    
    std::vector<std::string> explain() const override;
private:
    DocId _skip_to_nonexclude();
private:
    Scorer* _required_scorer{nullptr};
    Scorer* _exclude_scorer{nullptr};
    DocId _current_doc = -1;
};

} // namespace