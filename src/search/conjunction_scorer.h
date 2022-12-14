#pragma once

#include "query.h"
#include <vector>

namespace mmse {

// ConjunctionScorer用于求多条拉链的交集
 
class ConjunctionScorer : public Scorer {
public:
    ConjunctionScorer();
    ConjunctionScorer(std::vector<ScorerPtr>& scorers);
    ~ConjunctionScorer();

    ConjunctionScorer(ConjunctionScorer const&) = delete;             // Copy construct
    ConjunctionScorer(ConjunctionScorer&&) = delete;                  // Move construct
    ConjunctionScorer& operator=(ConjunctionScorer const&) = delete;  // Copy assign
    ConjunctionScorer& operator=(ConjunctionScorer &&) = delete;      // Move assign    

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;
    std::vector<std::string> explain() const override;
private:
    DocId _align_header();
private:
    std::vector<ScorerPtr> _scorers;
    DocId _last_doc = -1;
};

} // namespace